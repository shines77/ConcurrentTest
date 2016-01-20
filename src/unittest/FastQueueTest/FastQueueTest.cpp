
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include <type_traits>

#include <assert.h>

typedef double  jmc_timestamp_t;

namespace FastQueueTest {

enum benchmark_type_t {
    Boost_lockfree_queue,
    Intel_TBB_concurrent_queue,
    MoodyCamel_ConcurrentQueue,
    CXX11_Disruptor,

    // Max test queue type
    kMaxQueueType
};

enum queue_trait_value_t {
    kQueueDefaultCapacity = 1024,
    kCacheLineSize = 64
};

enum queue_op_state_t {
    OP_STATE_EMPTY = -2,
    OP_STATE_FAILURE = -1,
    OP_STATE_SUCCESS = 0
};

class Message {
public:
    uint64_t value;

public:
    Message() : value(0) {}
    Message(Message && src) {
        this->value = src.value;
    }
    ~Message() {}
};

namespace detail {

template <typename SizeType>
inline bool is_pow2(SizeType n) {
    typedef std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x = n;
    return ((x & (x - 1)) == 0);
}

template <typename SizeType>
inline SizeType check_is_pow2(SizeType n) {
    typedef std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x = n;
    return (x & (x - 1));
}

template <typename SizeType>
inline SizeType round_to_pow2(SizeType n) {
    typedef std::make_unsigned<SizeType>::type UnsignedSizeType;
    UnsignedSizeType x;
    if (is_pow2(n)) {
        return n;
    }
    else {
        x = static_cast<UnsignedSizeType>(n - 1);
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        return static_cast<SizeType>(x + 1);
    }
}

template <>
inline uint64_t round_to_pow2(uint64_t n) {
    uint64_t x;
    if (is_pow2(n)) {
        return n;
    }
    else {
        x = n - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        x = x | (x >> 32);
        return (x + 1);
    }
}

template <>
inline int64_t round_to_pow2(int64_t n) {
    uint64_t x;
    if (is_pow2(n)) {
        return n;
    }
    else {
        x = static_cast<uint64_t>(n - 1);
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        x = x | (x >> 32);
        return static_cast<int64_t>(x + 1);
    }
}

} // namespace detail

template <typename T, typename LockType = std::mutex,
          typename IndexType = uint64_t,
          size_t Capacity = kQueueDefaultCapacity>
class LockedRingQueue {
public:
    typedef T               item_type;
    typedef T *             value_type;
    typedef LockType        lock_type;
    typedef IndexType       index_type;
    typedef std::size_t     size_type;

    static const index_type kInitCursor = (index_type)(-1);
    static const index_type kDefaultCapacity = (index_type)kQueueDefaultCapacity;
    static const index_type kCapacity = (index_type)Capacity;
    static const index_type kIndexMask = (index_type)(kCapacity - 1);
    static const size_type kAlignment = kCacheLineSize;

private:
    index_type      head_;
    index_type      tail_;
    value_type      entries_;
    size_type       capacity_;
    value_type      allocEntries_;
    size_type       allocSize_;
    lock_type       lock_;

private:
    //

protected:
    inline void internal_init(size_type size) {
        static const size_type kAligntMask = ~(kAlignment - 1);
        static const size_type kAlignPadding = kAlignment - 1;
        // entries_ addr must align to kAlignment byte.
        size_type allocSize = size + kAlignPadding;
        value_type newEntries = new item_type[allocSize];
        if (newEntries) {
            allocEntries_ = newEntries;
            allocSize_ = allocSize;
            entries_ = reinterpret_cast<value_type>(reinterpret_cast<uintptr_t>(newEntries + kAlignPadding) & kAligntMask);
        }
    }

    inline void init(size_type size) {
        // entry sizes must align to power of 2.
        size_type sizeOfPow2 = detail::round_to_pow2<size_type>(size);
        internal_init(sizeOfPow2);
    }

public:
    LockedRingQueue(uint32_t size = kDefaultCapacity)
        : head_(kInitCursor), tail_(kInitCursor), capacity_(0),
          entries_(nullptr), allocEntries_(nullptr), allocSize_(0) {
        internal_init(size);
    }

    virtual ~LockedRingQueue() {
        lock_.lock();
        free_queue_fast();
        lock_.unlock();
    }

    inline void free_queue_fast() {
        if (allocEntries_) {
            delete[] allocEntries_;
            allocEntries_ = nullptr;
#if defined(_DEBUG) || !defined(NDEBUG)
            allocSize_ = 0;
            entries_ = nullptr;
#endif
        }
    }

    inline void free_queue() {
        if (allocEntries_) {
            delete[] allocEntries_;
            allocEntries_ = nullptr;
            allocSize_ = 0;
            entries_ = nullptr;
        }
    }

    void create(size_type size) {
        lock_.lock();
        free_queue();
        init(size);
        lock_.unlock();
    }

    void resize(size_type size) {
        lock_.lock();
        free_queue();
        init(size);
        lock_.unlock();
    }

    bool is_empty() const {
        bool _isEmpty;
        lock_.lock();
        _isEmpty = (head_ == tail_);
        lock_.unlock();
        return _isEmpty;
    }

    size_type sizes() const {
        size_type size;
        lock_.lock();
        size = head_ - tail_;
        lock_.unlock();
        assert(size <= kCapacity);
        return size;
    }

    T front() {
        //
    }

    T back() {
        //
    }

    int push_front(T && item) {
        lock_.lock();

        if ((head_ - tail_) > kCapacity) {
            lock_.unlock();
            return OP_STATE_EMPTY;
        }

        index_type next = head_ + 1;
        index_type index = next & kIndexMask;
        assert(entries_ != nullptr);
        entries_[index] = item;
        head_ = next;

        lock_.unlock();
        return OP_STATE_SUCCESS;
    }


    int pop_front(T & item) {
        lock_.lock();

        if (head_ == tail_) {
            lock_.unlock();
            return OP_STATE_EMPTY;
        }

        index_type next = tail_ + 1;
        index_type index = next & kIndexMask;
        assert(entries_ != nullptr);
        item = entries_[index];
        tail_ = next;

        lock_.unlock();
        return OP_STATE_SUCCESS;
    }

    int push_front_ref(const T & item);
    int pop_front_ref(T & item);

    int push_front_point(const T * item);
    int pop_front_point(T * item);
};

template <typename T, typename IndexType = uint64_t, size_t Capacity = kQueueDefaultCapacity>
using StdMutexRingQueue = typename LockedRingQueue<T, std::mutex, IndexType, Capacity>;

template <typename QueueType, uintptr_t QueueTrait>
jmc_timestamp_t runBenchmark(benchmark_type_t benchmark, uintptr_t messages, uintptr_t iterators, uintptr_t threads)
{
    jmc_timestamp_t startTime, elapsedTime;
    elapsedTime = 0.0 - startTime;
    return elapsedTime;
}

} // namespace ConcourrentTest

#define ASSERT_BOOLEAN_TRUE(infoTrue, infoFalse, expr) \
    do {                                    \
        if (!!(expr)) {                     \
            printf(""##infoTrue##".\n");    \
        }                                   \
        else {                              \
            printf(""##infoFalse##".\n");   \
        }                                   \
    } while (0)

#define ASSERT_BOOLEAN_TRUE_EX(infoTrue, infoFalse, name, expr) \
    do {                                             \
        if (!!(expr)) {                              \
            printf("%s : "##infoTrue##".\n", name);  \
        }                                            \
        else {                                       \
            printf("%s : "##infoFalse##".\n", name); \
        }                                            \
    } while (0)

#define ASSERT_BOOLEAN_VERIFY(expr) \
    ASSERT_BOOLEAN_TRUE("Correct", "Error", expr)

#define ASSERT_BOOLEAN_VERIFY_EX(name, expr) \
    ASSERT_BOOLEAN_TRUE_EX("Correct", "Error", name, expr)

#define ASSERT_BOOLEAN_VALUE(expr) \
    ASSERT_BOOLEAN_TRUE("True", "False", expr)

#define ASSERT_BOOLEAN_VALUE_EX(name, expr) \
    ASSERT_BOOLEAN_TRUE_EX("True", "False", name, expr)

#define ASSERT_BOOLEAN_PASSED(expr) \
    ASSERT_BOOLEAN_TRUE("Passed", "Failure", expr)

#define ASSERT_BOOLEAN_PASSED_EX(name, expr) \
    ASSERT_BOOLEAN_TRUE_EX("Passed", "Failure", name, expr)

#define ASSERT_BOOLEAN_SUCCESS(expr) \
    ASSERT_BOOLEAN_TRUE("Success", "Failure", expr)

#define ASSERT_BOOLEAN_SUCCESS_EX(name, expr) \
    ASSERT_BOOLEAN_TRUE_EX("Success", "Failure", name, expr)

#define DISPLAY_VALUE(fmt, __VARS__)  printf(fmt, __VARS__)

using namespace FastQueueTest;

int main(int argc, char * argv[])
{
    LockedRingQueue<Message, std::mutex, uint32_t, 1024> lockedRingQueue;
    StdMutexRingQueue<Message, uint32_t, 1024> stdMutexRingQueue;

    Message message;
    int success;

    message.value = 1;
    success = lockedRingQueue.push_front(std::move(message));
    ASSERT_BOOLEAN_SUCCESS_EX("LockedRingQueue::push_front()", success == OP_STATE_SUCCESS);
    ASSERT_BOOLEAN_PASSED_EX("LockedRingQueue::push_front()", message.value == 1);
    message.value = 2;
    success = lockedRingQueue.pop_front(message);
    ASSERT_BOOLEAN_SUCCESS_EX("LockedRingQueue::pop_front()", success == OP_STATE_SUCCESS);
    ASSERT_BOOLEAN_PASSED_EX("LockedRingQueue::pop_front()", message.value == 1);

    message.value = 1;
    success = stdMutexRingQueue.push_front(std::move(message));
    ASSERT_BOOLEAN_SUCCESS_EX("StdMutexRingQueue::push_front()", success == OP_STATE_SUCCESS);
    ASSERT_BOOLEAN_PASSED_EX("StdMutexRingQueue::push_front()", message.value == 1);
    message.value = 2;
    success = stdMutexRingQueue.pop_front(message);
    ASSERT_BOOLEAN_SUCCESS_EX("StdMutexRingQueue::pop_front()", success == OP_STATE_SUCCESS);
    ASSERT_BOOLEAN_PASSED_EX("StdMutexRingQueue::pop_front()", message.value == 1);
    
    bool isPow2;
    isPow2 = detail::is_pow2(0xFFFFFFFFUL);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(0xFFFFFFFFUL)", isPow2 == false);
    isPow2 = detail::is_pow2(0);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(0)", isPow2 == true);
    isPow2 = detail::is_pow2(1);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(1)", isPow2 == true);
    isPow2 = detail::is_pow2(2);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(2)", isPow2 == true);
    isPow2 = detail::is_pow2(3);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(3)", isPow2 == false);
    isPow2 = detail::is_pow2(4);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(4)", isPow2 == true);
    isPow2 = detail::is_pow2(16);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(16)", isPow2 == true);
    isPow2 = detail::is_pow2(17);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(17)", isPow2 == false);
    isPow2 = detail::is_pow2(37);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(37)", isPow2 == false);

    size_t checkVal;
    checkVal = detail::check_is_pow2(0xFFFFFFFFUL);
    printf("is_pow2(0xFFFFFFFFUL) : %d\n", checkVal);
    checkVal = detail::check_is_pow2(0);
    printf("is_pow2(0) : %d\n", checkVal);
    checkVal = detail::check_is_pow2(1);
    printf("is_pow2(1) : %d\n", checkVal);
    checkVal = detail::check_is_pow2(2);
    printf("is_pow2(2) : %d\n", checkVal);
    checkVal = detail::check_is_pow2(3);
    printf("is_pow2(3) : %d\n", checkVal);
    checkVal = detail::check_is_pow2(4);
    printf("is_pow2(4) : %d\n", checkVal);
    checkVal = detail::check_is_pow2(16);
    printf("is_pow2(16) : %d\n", checkVal);
    checkVal = detail::check_is_pow2(17);
    printf("is_pow2(17) : %d\n", checkVal);
    checkVal = detail::check_is_pow2(37);
    printf("is_pow2(37) : %d\n", checkVal);

    printf("\n");

    std::atomic<int> atomic1;
    int lock_status = 0;
    int new_status = atomic1.compare_exchange_strong(lock_status, 1, std::memory_order_seq_cst);
    printf("new_status = %d\n", new_status);

    printf("\n");
    ::system("pause");
    return 0;
}
