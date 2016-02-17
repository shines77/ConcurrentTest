
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <deque>
#include <type_traits>

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) \
 || defined(_WINDOWS) || defined(__MINGW__) || defined(__MINGW32__)
#include <windows.h>
//#include <minwinbase.h>
//#include <synchapi.h>
#else
#include <pthread.h>
#endif
#include <assert.h>
#include <immintrin.h>

#include <FastQueue/queue/LockedRingQueue.h>

using namespace FastQueue;

#if defined(NDEBUG)
#define MAX_MESSAGE_COUNT   (800 * 10000)
#else
#define MAX_MESSAGE_COUNT   (50 * 10000)
#endif

#if defined(NDEBUG)
#define MAX_ITERATIONS      (200 * 100000)
#else
#define MAX_ITERATIONS      (10 * 100000)
#endif

typedef double  jmc_timestamp_t;

enum {
    kMaxMessageCount = MAX_MESSAGE_COUNT
};

enum benchmark_type_t {
    Boost_lockfree_queue,
    Intel_TBB_concurrent_queue,
    MoodyCamel_ConcurrentQueue,
    CXX11_Disruptor,

    // Max test queue type
    kMaxQueueType
};

class Message {
public:
    uint64_t value;

public:
    Message() : value(0) {}
    Message(Message const & src) {
        this->value = src.value;
    }
    Message(Message && src) {
        this->value = src.value;
    }
    ~Message() {}

    Message & operator = (const Message & rhs) {
        this->value = rhs.value;
        return *this;
    }
};

#define JIMI_DEFAULT_METHOD     default
#define JIMI_DELETE_METHOD      delete

namespace local {

template <typename T, bool HasLocked = false>
class scoped_lock {
private:
    typedef T mutex_type;
    mutex_type & mutex_;

    scoped_lock(mutex_type const &) = JIMI_DELETE_METHOD;
    scoped_lock & operator = (mutex_type const &) = JIMI_DELETE_METHOD;

public:
    explicit scoped_lock(mutex_type & mutex) : mutex_(mutex) {
        if (!HasLocked) {
            mutex_.lock();
        }
    }

    ~scoped_lock() {
        mutex_.unlock();
    }
};

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) \
 || defined(_WINDOWS) || defined(__MINGW__) || defined(__MINGW32__)
class Mutex {
private:
  CRITICAL_SECTION  mutex_;

public:
  Mutex() {
    ::InitializeCriticalSection(&mutex_);
  }

  ~Mutex() {
    ::DeleteCriticalSection(&mutex_);
  }

  void lock() {
    ::EnterCriticalSection(&mutex_);
  }

  bool try_lock() {
    return (::TryEnterCriticalSection(&mutex_) != 0);
  }

  void unlock() {
    ::LeaveCriticalSection(&mutex_);
  }
};
#elif defined(__linux__) || defined(__CYGWIN__) || defined(PTHREAD_H) || defined(_PTHREAD_H)
class Mutex {
private:
  pthread_mutex_t  mutex_;

public:
  Mutex() {
    pthread_mutex_init(&mutex_);
  }

  ~Mutex() {
    pthread_mutex_destroy(&mutex_);
  }

  void lock() {
    pthread_mutex_lock(&mutex_);
  }

  bool try_lock() {
    return pthread_mutex_trylock(&mutex_);
  }

  void unlock() {
    pthread_mutex_unlock(&mutex_);
  }
};
#else // Other OS
#error WTF
#endif // _WIN32

} // namespace local

template <typename T>
class QueueWrapper {
public:
    QueueWrapper() {}
    ~QueueWrapper() {}
};

template <typename T, typename MutexType>
class StdQueueWrapper : public QueueWrapper< StdQueueWrapper<T, MutexType> > {
public:
    typedef std::queue<T>   queue_type;
    typedef T               item_type;
    typedef MutexType       mutex_type;

private:
    mutex_type mutex_;
    queue_type queue_;

public:
    StdQueueWrapper() : mutex_(), queue_() {}
    ~StdQueueWrapper() {}

    bool empty() const {
        local::scoped_lock<mutex_type> lock(mutex_);
        bool is_empty = queue_.empty();
        return is_empty;
    }

    size_t sizes() const {
        local::scoped_lock<mutex_type> lock(mutex_);
        size_t size = queue_.sizes();
        return size;
    }

    void resize(size_t new_size) {
        // Do nothing!!
    }

    template <typename U>
    void push(U && item) {
        local::scoped_lock<mutex_type> lock(mutex_);
        queue_.push(std::forward<U>(item));
    }

    item_type & front() {
        local::scoped_lock<mutex_type> lock(mutex_);
        item_type & item = queue_.front();
        return item;
    }

    item_type & back() {
        local::scoped_lock<mutex_type> lock(mutex_);
        item_type & item = queue_.back();
        return item;
    }

    void pop() {
        local::scoped_lock<mutex_type> lock(mutex_);
        queue_.pop();
    }

    int pop(item_type & item) {
        local::scoped_lock<mutex_type> lock(mutex_);
        if (!queue_.empty()) {
            item_type & ret = queue_.back();
            item = std::move(ret);
            queue_.pop();
            return true;
        }
        else {
            return false;
        }
    }
};

template <typename T, typename MutexType>
class StdDequeueWrapper : public QueueWrapper< StdDequeueWrapper<T, MutexType> > {
public:
    typedef std::deque<T>   queue_type;
    typedef T               item_type;
    typedef MutexType       mutex_type;

private:
    mutex_type mutex_;
    queue_type queue_;

public:
    StdDequeueWrapper() : mutex_(), queue_() {}
    ~StdDequeueWrapper() {}

    bool empty() const {
        local::scoped_lock<mutex_type> lock(mutex_);
        bool is_empty = queue_.empty();
        return is_empty;
    }

    size_t sizes() const {
        local::scoped_lock<mutex_type> lock(mutex_);
        size_t size = queue_.sizes();
        return size;
    }

    void resize(size_t new_size) {
        local::scoped_lock<mutex_type> lock(mutex_);
        queue_.resize(new_size);
    }

    template <typename U>
    void push(U && item) {
        local::scoped_lock<mutex_type> lock(mutex_);
        queue_.push_back(std::forward<U>(item));
    }

    item_type & front() {
        local::scoped_lock<mutex_type> lock(mutex_);
        item_type & item = queue_.front();
        return item;
    }

    item_type & back() {
        local::scoped_lock<mutex_type> lock(mutex_);
        item_type & item = queue_.back();
        return item;
    }

    void pop() {
        local::scoped_lock<mutex_type> lock(mutex_);
        queue_.pop_front();
    }

    int pop(item_type & item) {
        local::scoped_lock<mutex_type> lock(mutex_);
        if (!queue_.empty()) {
            item_type & ret = queue_.back();
            item = std::move(ret);
            queue_.pop_front();
            return true;
        }
        else {
            return false;
        }
    }
};

template <typename T, typename MutexType, typename IndexType>
class LockedRingQueueWrapper
    : public QueueWrapper< LockedRingQueueWrapper<T, MutexType, IndexType> > {
public:
    typedef LockedRingQueue<T, MutexType, IndexType> queue_type;
    typedef T item_type;

private:
    queue_type queue_;

public:
    LockedRingQueueWrapper() : queue_() {}
    ~LockedRingQueueWrapper() {}

    bool empty() const {
        return queue_.is_empty();
    }

    size_t sizes() const {
        return queue_.sizes();
    }

    void resize(size_t new_size) {
        return queue_.resize(new_size);
    }

    template <typename U>
    void push(U && item) {
        queue_.push_front(std::forward<U>(item));
    }

    item_type & back() {
        item_type item;
        if (queue_.pop_back(item) == QUEUE_OP_SUCCESS) {
            return std::move(item);
        }
        else {
            throw ("LockedRingQueue<T> is empty!");
        }
    }

    void pop() {
        item_type item;
        queue_.pop_back(item);
    }

    int pop(item_type & item) {
        return queue_.pop_back(item);
    }
};

template <typename T, typename MutexType, typename IndexType,
          size_t InitCapacity = kQueueDefaultCapacity>
class FixedLockedRingQueueWrapper
    : public QueueWrapper< FixedLockedRingQueueWrapper<T, MutexType, IndexType, InitCapacity> > {
public:
    typedef FixedLockedRingQueue<T, MutexType, IndexType, InitCapacity> queue_type;
    typedef T item_type;

private:
    queue_type queue_;

public:
    FixedLockedRingQueueWrapper() : queue_() {}
    ~FixedLockedRingQueueWrapper() {}

    bool empty() const {
        return queue_.is_empty();
    }

    size_t sizes() const {
        return queue_.sizes();
    }

    void resize(size_t new_size) {
        return queue_.resize(new_size);
    }

    template <typename U>
    void push(U && item) {
        queue_.push_front(std::forward<U>(item));
    }

    item_type & back() {
        item_type item;
        if (queue_.pop_back(item) == QUEUE_OP_SUCCESS) {
            return std::move(item);
        }
        else {
            throw ("FixedLockedRingQueue<T> is empty!");
        }
    }

    void pop() {
        item_type item;
        queue_.pop_back(item);
    }

    int pop(item_type & item) {
        return queue_.pop_back(item);
    }
};

template <typename QueueType, typename MessageType>
void producer_thread_proc(unsigned index, unsigned producers, QueueType * queue)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    //printf("Producer Thread: thread_idx = %d, producers = %d.\n", index, producers);

    unsigned messages = kMaxMessageCount / producers;
    for (unsigned i = 0; i < messages; ++i) {
        message_type * msg = new message_type();
        queue->push(msg);
    }
}

template <typename QueueType, typename MessageType>
void consumer_thread_proc(unsigned index, unsigned consumers, QueueType * queue)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    //printf("Consumer Thread: thread_idx = %d, consumers = %d.\n", index, consumers);

    unsigned messages = kMaxMessageCount / consumers;
    for (unsigned i = 0; i < messages; ++i) {
#if 0
        if (!queue->empty()) {
            message_type *& msg = queue->back();
            queue->pop();
        }
#else
        message_type * msg = nullptr;
        queue->pop(msg);
#endif
    }
}

template <typename QueueType, typename MessageType>
void run_test_threads(unsigned producers, unsigned consumers, size_t initCapacity)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    queue_type queue;
    queue.resize(initCapacity);

    std::thread ** producer_threads = new std::thread *[producers];
    std::thread ** consumer_threads = new std::thread *[consumers];

    if (producer_threads) {
        for (unsigned i = 0; i < producers; ++i) {
            std::thread * thread = new std::thread(producer_thread_proc<queue_type, message_type>,
                i, producers, &queue);
            producer_threads[i] = thread;
        }
    }

    if (consumer_threads) {
        for (unsigned i = 0; i < consumers; ++i) {
            std::thread * thread = new std::thread(consumer_thread_proc<queue_type, message_type>,
                i, consumers, &queue);
            consumer_threads[i] = thread;
        }
    }

    if (producer_threads) {
        for (unsigned i = 0; i < producers; ++i) {
            producer_threads[i]->join();
        }
    }

    if (consumer_threads) {
        for (unsigned i = 0; i < consumers; ++i) {
            consumer_threads[i]->join();
        }
    }

    if (producer_threads) {
        for (unsigned i = 0; i < producers; ++i) {
            if (producer_threads[i])
                delete producer_threads[i];
        }
        delete[] producer_threads;
    }

    if (consumer_threads) {
        for (unsigned i = 0; i < consumers; ++i) {
            if (consumer_threads[i])
                delete consumer_threads[i];
        }
        delete[] consumer_threads;
    }
}

template <typename QueueType, typename MessageType>
void run_test(unsigned producers, unsigned consumers, size_t initCapacity)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    printf("Test for: %s\n", typeid(queue_type).name());
    printf("\n");

    using namespace std::chrono;
    time_point<high_resolution_clock> startime = high_resolution_clock::now();

    run_test_threads<queue_type, message_type>(producers, consumers, initCapacity);

    time_point<high_resolution_clock> endtime = high_resolution_clock::now();
    duration<double> elapsed_time = duration_cast< duration<double> >(endtime - startime);

    //printf("\n");
    printf("Elapsed time: %0.3f second(s)\n", elapsed_time.count());
    printf("Throughput:   %0.1f op/sec\n", (double)(kMaxMessageCount * 1) / elapsed_time.count());
    printf("\n");
}

std::atomic<long> g_counter = 0;
volatile long g_cntval = 0;

enum {
    test_suit_atomic_fetch_add,
    test_suit_x86_lock_xadd,
    test_suit_x86_xchg,
    test_suit_x86_xchg_cnt_error,
    test_suit_x86_HLE_xchg,
    test_suit_x86_lock_cmp_xchg,
    test_suit_x86_lock_cmp_xchg_cnt_error,
};

const char * getTestSuiteName(size_t test_suite)
{
    switch (test_suite) {
        case test_suit_atomic_fetch_add:
            return "test_suit_atomic_fetch_add";
        case test_suit_x86_lock_xadd:
            return "test_suit_x86_lock_xadd";
        case test_suit_x86_xchg:
            return "test_suit_x86_xchg";
        case test_suit_x86_xchg_cnt_error:
            return "test_suit_x86_xchg_cnt_error";
        case test_suit_x86_HLE_xchg:
            return "test_suit_x86_HLE_xchg";
        case test_suit_x86_lock_cmp_xchg:
            return "test_suit_x86_lock_cmp_xchg";
        case test_suit_x86_lock_cmp_xchg_cnt_error:
            return "test_suit_x86_lock_cmp_xchg_cnt_error";
        default:
            return "unknown_test_suite_name";
    }
}

template <size_t test_suite>
void atomic_test_thread_proc(unsigned thread_idx, unsigned nthread, unsigned iterations) { }

template <>
void atomic_test_thread_proc<test_suit_atomic_fetch_add>(unsigned thread_idx, unsigned nthread, unsigned iterations)
{
    for (unsigned i = 0; i < iterations; ++i) {
        g_counter.fetch_add(1, std::memory_order_acq_rel);
    }
}

template <>
void atomic_test_thread_proc<test_suit_x86_lock_xadd>(unsigned thread_idx, unsigned nthread, unsigned iterations)
{
    for (unsigned i = 0; i < iterations; ++i) {
        ::_InterlockedExchangeAdd(&g_cntval, 1);
    }
}

template <>
void atomic_test_thread_proc<test_suit_x86_xchg>(unsigned thread_idx, unsigned nthread, unsigned iterations)
{
    for (unsigned i = 0; i < iterations; ++i) {
        long old_value, new_value, now_value;
        do {
            _ReadWriteBarrier();
            old_value = g_cntval;
            _ReadWriteBarrier();
            new_value = old_value + 1;
            now_value = ::_InterlockedExchange(&g_cntval, new_value);
        } while (now_value != old_value);
    }
}

template <>
void atomic_test_thread_proc<test_suit_x86_xchg_cnt_error>(unsigned thread_idx, unsigned nthread, unsigned iterations)
{
    unsigned s_errors = 0, s_collects = 0;
    for (unsigned i = 0; i < iterations; ++i) {
        long old_value, new_value, now_value;
        do {
            _ReadWriteBarrier();
            old_value = g_cntval;
            _ReadWriteBarrier();
            new_value = old_value + 1;
            now_value = ::_InterlockedExchange(&g_cntval, new_value);
            if (now_value != old_value) {
                s_errors++;
            }
            else {
                s_collects++;
            }
        } while (now_value != old_value);
    }

    printf("thread_idx = %u, s_collects = %u, s_errors = %u\n", thread_idx, s_collects, s_errors);
}

template <>
void atomic_test_thread_proc<test_suit_x86_HLE_xchg>(unsigned thread_idx, unsigned nthread, unsigned iterations)
{
    for (unsigned i = 0; i < iterations; ++i) {
        long old_value, new_value, now_value;
        do {
            _ReadWriteBarrier();
            old_value = g_cntval;
            _ReadWriteBarrier();
            new_value = old_value + 1;
            now_value = ::_InterlockedExchange_HLEAcquire(&g_cntval, new_value);
            ::_InterlockedExchange_HLERelease(&g_cntval, new_value);
        } while (now_value != old_value);
    }
}

template <>
void atomic_test_thread_proc<test_suit_x86_lock_cmp_xchg>(unsigned thread_idx, unsigned nthread, unsigned iterations)
{
    for (unsigned i = 0; i < iterations; ++i) {
        long old_value, new_value, now_value;
        do {
            _ReadWriteBarrier();
            old_value = g_cntval;
            _ReadWriteBarrier();
            new_value = old_value + 1;
            now_value = ::_InterlockedCompareExchange(&g_cntval, new_value, old_value);
        } while (now_value != old_value);
    }
}

template <>
void atomic_test_thread_proc<test_suit_x86_lock_cmp_xchg_cnt_error>(unsigned thread_idx, unsigned nthread, unsigned iterations)
{
    unsigned s_errors = 0, s_collects = 0;
    for (unsigned i = 0; i < iterations; ++i) {
        long old_value, new_value, now_value;
        do {
            _ReadWriteBarrier();
            old_value = g_cntval;
            _ReadWriteBarrier();
            new_value = old_value + 1;
            now_value = ::_InterlockedCompareExchange(&g_cntval, new_value, old_value);
            if (now_value != old_value) {
                s_errors++;
            }
            else {
                s_collects++;
            }
        } while (now_value != old_value);
    }

    printf("thread_idx = %u, s_collects = %u, s_errors = %u\n", thread_idx, s_collects, s_errors);
}

template <size_t test_suite>
void run_atomic_test_threads(unsigned nthread, unsigned total_iterations)
{
    std::thread ** atomic_test_threads = new std::thread *[nthread];

    unsigned iterations = total_iterations / nthread;
    if (atomic_test_threads) {
        for (unsigned i = 0; i < nthread; ++i) {
            std::thread * thread = new std::thread(atomic_test_thread_proc<test_suite>,
                i, nthread, iterations);
            atomic_test_threads[i] = thread;
        }
    }

    if (atomic_test_threads) {
        for (unsigned i = 0; i < nthread; ++i) {
            atomic_test_threads[i]->join();
        }
    }

    if (atomic_test_threads) {
        for (unsigned i = 0; i < nthread; ++i) {
            if (atomic_test_threads[i])
                delete atomic_test_threads[i];
        }
        delete[] atomic_test_threads;
    }
}

template <size_t test_suite>
void run_atomic_test(unsigned nthread, unsigned total_iterations)
{
    g_counter = 0;
    g_cntval = 0;

    printf("Test for: run_atomic_test<%s>(%u)\n", getTestSuiteName(test_suite), nthread);
    printf("\n");

    using namespace std::chrono;
    time_point<high_resolution_clock> startime = high_resolution_clock::now();

    run_atomic_test_threads<test_suite>(nthread, total_iterations);

    time_point<high_resolution_clock> endtime = high_resolution_clock::now();
    duration<double> elapsed_time = duration_cast< duration<double> >(endtime - startime);

    printf("nthread = %d\n", nthread);
    printf("iterations = %d\n", total_iterations / nthread);
    printf("\n");
    printf("g_counter = %d\n", g_counter.load(std::memory_order_seq_cst));
    printf("g_cntval = %d\n", g_cntval);

    printf("\n");
    printf("elapsed time: %0.3f second(s)\n", elapsed_time.count());
    printf("\n");
    printf("--------------------------------------------------------------\n");
}

int main(int argc, char * argv[])
{
    unsigned producers, consumers;
    producers = 1;
    consumers = 1;

    printf("ConcurrentTest.\n");
    printf("\n");
    printf("Messages  = %u\n", kMaxMessageCount);
    printf("Producers = %u\n", producers);
    printf("Consumers = %u\n", consumers);
#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) \
 || defined(__amd64__) || defined(__x86_64__)
    printf("x86_64    = true\n");
#else
    printf("x86_64    = false\n");
#endif
    printf("\n");

#if 1
    printf("-------------------------------------------------------------------------\n");

    run_test<StdQueueWrapper<Message *, local::Mutex>, Message>(producers, consumers, 4096);
    run_test<StdDequeueWrapper<Message *, local::Mutex>, Message>(producers, consumers, 4096);

    run_test<LockedRingQueueWrapper<Message *, local::Mutex, uint64_t>, Message>(producers, consumers, 4096);
    run_test<FixedLockedRingQueueWrapper<Message *, local::Mutex, uint64_t, 4096>, Message>(producers, consumers, 4096);

    printf("-------------------------------------------------------------------------\n");

    run_test<StdQueueWrapper<Message *, std::mutex>, Message>(producers, consumers, 4096);
    run_test<StdDequeueWrapper<Message *, std::mutex>, Message>(producers, consumers, 4096);

    run_test<LockedRingQueueWrapper<Message *, std::mutex, uint64_t>, Message>(producers, consumers, 4096);
    run_test<FixedLockedRingQueueWrapper<Message *, std::mutex, uint64_t, 4096>, Message>(producers, consumers, 4096);

    printf("-------------------------------------------------------------------------\n");
#endif

#if 0
    for (unsigned nthread = 4; nthread <= 16; nthread *= 2) {
        run_atomic_test<test_suit_atomic_fetch_add>(nthread, MAX_ITERATIONS);
        run_atomic_test<test_suit_x86_lock_xadd>(nthread, MAX_ITERATIONS);
        run_atomic_test<test_suit_x86_xchg>(nthread, MAX_ITERATIONS);
        run_atomic_test<test_suit_x86_xchg_cnt_error>(nthread, MAX_ITERATIONS);
        run_atomic_test<test_suit_x86_HLE_xchg>(nthread, MAX_ITERATIONS);
        run_atomic_test<test_suit_x86_lock_cmp_xchg>(nthread, MAX_ITERATIONS);
        run_atomic_test<test_suit_x86_lock_cmp_xchg_cnt_error>(nthread, MAX_ITERATIONS);
    }
#endif

    printf("\n");
    ::system("pause");
    return 0;
}
