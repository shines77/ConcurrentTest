
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>

using namespace ConcourrentTest;

typedef double  jmc_timestamp_t;

namespace ConcourrentTest {

enum benchmark_type_t {
    Boost_lockfree_queue,
    Intel_TBB_concurrent_queue,
    MoodyCamel_ConcurrentQueue,
    CXX11_Disruptor,

    // Max test queue type
    kMaxQueueType
};

enum queue_traist {
    kQueueDefaultCapacity = 16,
    kCacheLineSize = 64
};

template <typename T, typename LockType, typename IndexType,
    size_t Capacity = kQueueDefaultCapacity>
class RingBufferQueue {
public:
    typedef T               item_type;
    typedef T *             value_type;
    typedef LockType        lock_type;
    typedef IndexType       index_type;
    typedef std::size_t     size_type;

    enum op_type_t {
        STATUS_EMPTY = -1,
        STATUS_SUCCESS = 0
    };

    static const index_type kInitCursor = (index_type)-1;
    static const index_type kDefaultCapacity = (index_type)kQueueDefaultCapacity;
    static const index_type kCapacity = (index_type)Capacity;
    static const size_type kAlignment = kCacheLineSize;

protected:
    void init(size_type size) {
        // entries_ addr must align to kAlignment byte.
        size_type allocSize = size + kAlignment - 1;
        if (entries_) {
            value_type newEntries = new item_type[allocSize];
            entries_ = newEntries & (kAlignment - 1);
        }
    }

public:
    RingBufferQueue(uint32_t size = kDefaultCapacity) : head_(kInitCursor), tail_(kInitCursor), entries_(nullptr) {
        init(size);
    }

    virtual ~RingBufferQueue() {
        free_queue();
    }

    void free_queue() {
        if (entries_) {
            delete[] entries_;
            entries_ = nullptr;
        }
    }

    void create(size_type size) {
        init(size);
    }

    T front() {
        //
    }

    T back() {
        //
    }

    int push_front(T && item) {
        lock_.lock();

        if (head_ == tail_ || (head_ - tail_) > kCapacity) {
            lock_.unlock();
            return STATUS_EMPTY;
        }

        index_type next = head_ + 1;
        entries_[next] = item;
        head_ = next;

        lock_.unlock();
        return STATUS_SUCCESS;
    }


    int pop_front(T & item) {
        lock_.lock();

        if (head_ == tail_ || (head_ - tail_) > kCapacity) {
            lock_.unlock();
            return STATUS_EMPTY;
        }

        index_type next = tail_ + 1;
        entries_[tail_] = item;
        tail_ = next;

        lock_.unlock();
        return STATUS_SUCCESS;
    }

    int push_ref(const T & item);
    int pop_ref(const T & item);

    int push_point(const T * item);
    T * pop_point();

private:
    index_type      head_;
    index_type      tail_;
    size_type       capacity_;
    lock_type       lock_;
    value_type      entries_;
};

template <typename T, uint32_t _CapacitySize>
class StdMutexRingQueue {
private:
    std::mutex lock_;
};

template <typename QueueType, uintptr_t QueueTraist>
jmc_timestamp_t runBenchmark(benchmark_type_t benchmark, uintptr_t messages, uintptr_t iterators, uintptr_t threads)
{
    jmc_timestamp_t startTime, elapsedTime;
    elapsedTime = 0.0 - startTime;
    return elapsedTime;
}

} // namespace ConcourrentTest

int main(int argc, char * argv[])
{
    return 0;
}
