
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

#include <assert.h>

#include <FastQueue/queue/LockedRingQueue.h>

using namespace FastQueue;

#if defined(NDEBUG)
#define MAX_MESSAGE_COUNT   (800 * 10000)
#else
#define MAX_MESSAGE_COUNT   (50 * 10000)
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

template <typename T>
class QueueWapper {
public:
    QueueWapper() {}
    ~QueueWapper() {}
};

template <typename T>
class StdQueueWapper : public QueueWapper< std::queue<T> > {
public:
    typedef std::queue<T>   queue_type;
    typedef T               item_type;

private:
    std::mutex lock_;
    queue_type queue_;

public:
    StdQueueWapper() : lock_(), queue_() {}
    ~StdQueueWapper() {}

    bool empty() const {
        bool is_empty;
        lock_.lock();
        is_empty = queue_.empty();
        lock_.unlock();
        return is_empty;
    }

    size_t sizes() const {
        size_t size;
        lock_.lock();
        size = queue_.sizes();
        lock_.unlock();
        return size;
    }

    void resize(size_t new_size) {
        // Do nothing!!
    }

    void push(item_type const & item) {
        lock_.lock();
        queue_.push(item);
        lock_.unlock();
    }

    void push(item_type && item) {
        lock_.lock();
        queue_.push(item);
        lock_.unlock();
    }

    item_type & back() {
        lock_.lock();
        item_type & item = queue_.back();
        lock_.unlock();
        return item;
    }

    void pop() {
        lock_.lock();
        queue_.pop();
        lock_.unlock();
    }

    int pop(item_type & item) {
        lock_.lock();
        if (!queue_.empty()) {
            item_type & ret = queue_.back();
            item = std::move(ret);
            queue_.pop();
            lock_.unlock();
            return true;
        }
        else {
            lock_.unlock();
            return false;
        }
    }
};

template <typename T>
class StdDequeueWapper : public QueueWapper< std::deque<T> > {
public:
    typedef std::deque<T>   queue_type;
    typedef T               item_type;

private:
    std::mutex lock_;
    queue_type queue_;

public:
    StdDequeueWapper() : lock_(), queue_() {}
    ~StdDequeueWapper() {}

    bool empty() const {
        bool is_empty;
        lock_.lock();
        is_empty = queue_.empty();
        lock_.unlock();
        return is_empty;
    }

    size_t sizes() const {
        size_t size;
        lock_.lock();
        size = queue_.sizes();
        lock_.unlock();
        return size;
    }

    void resize(size_t new_size) {
        lock_.lock();
        queue_.resize(new_size);
        lock_.unlock();
    }

    void push(item_type const & item) {
        lock_.lock();
        queue_.push_front(item);
        lock_.unlock();
    }

    void push(item_type && item) {
        lock_.lock();
        queue_.push_front(item);
        lock_.unlock();
    }

    item_type & back() {
        lock_.lock();
        item_type & item = queue_.back();
        lock_.unlock();
        return item;
    }

    void pop() {
        lock_.lock();
        queue_.pop_back();
        lock_.unlock();
    }

    int pop(item_type & item) {
        lock_.lock();
        if (!queue_.empty()) {
            item_type & ret = queue_.back();
            item = std::move(ret);
            queue_.pop_back();
            lock_.unlock();
            return true;
        }
        else {
            lock_.unlock();
            return false;
        }
    }
};

template <typename T>
class LockedRingQueueWapper : public QueueWapper< LockedRingQueue<T, std::mutex, uint64_t> > {
public:
    typedef LockedRingQueue<T, std::mutex, uint64_t>    queue_type;
    typedef T                                           item_type;

private:
    queue_type queue;

public:
    LockedRingQueueWapper() : queue() {}
    ~LockedRingQueueWapper() {}

    bool empty() const {
        return queue.is_empty();
    }

    size_t sizes() const {
        return queue.sizes();
    }

    void resize(size_t new_size) {
        return queue.resize(new_size);
    }

    void push(item_type const & item) {
        queue.push_front(item);
    }

    void push(item_type && item) {
        queue.push_front(item);
    }

    item_type & back() {
        item_type item;
        if (queue.pop_back(item) == OP_STATE_SUCCESS) {
            return std::move(item);
        }
        else {
            throw ("LockedRingQueue<T> is empty!");
        }
    }

    void pop() {
        item_type item;
        queue.pop_back(item);
    }

    int pop(item_type & item) {
        return queue.pop_back(item);
    }
};

template <typename T>
class FixedLockedRingQueueWapper : public QueueWapper< FixedLockedRingQueue<T, std::mutex, uint64_t> > {
public:
    typedef FixedLockedRingQueue<T, std::mutex, uint64_t, 4096> queue_type;
    typedef T                                                   item_type;

private:
    queue_type queue;

public:
    FixedLockedRingQueueWapper() : queue() {}
    ~FixedLockedRingQueueWapper() {}

    bool empty() const {
        return queue.is_empty();
    }

    size_t sizes() const {
        return queue.sizes();
    }

    void resize(size_t new_size) {
        return queue.resize(new_size);
    }

    void push(item_type const & item) {
        queue.push_front(item);
    }

    void push(item_type && item) {
        queue.push_front(item);
    }

    item_type & back() {
        item_type item;
        if (queue.pop_back(item) == OP_STATE_SUCCESS) {
            return std::move(item);
        }
        else {
            throw ("FixedLockedRingQueue<T> is empty!");
        }
    }

    void pop() {
        item_type item;
        queue.pop_back(item);
    }

    int pop(item_type & item) {
        return queue.pop_back(item);
    }
};

template <typename QueueType, typename MessageType>
void producer_thread_proc(unsigned index, unsigned producers, QueueType * queue)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    printf("Producer Thread: thread_idx = %d, producers = %d.\n", index, producers);

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

    printf("Consumer Thread: thread_idx = %d, consumers = %d.\n", index, consumers);

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

template <typename QueueType, typename MessageType, size_t InitSize>
void run_test_threads(unsigned producers, unsigned consumers)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    queue_type queue;
    queue.resize(InitSize);

    std::thread  ** producer_threads = new std::thread *[producers];
    std::thread  ** consumer_threads = new std::thread *[consumers];

    if (producer_threads) {
        for (unsigned i = 0; i < producers; ++i) {
            std::thread * thread = new std::thread(&producer_thread_proc<queue_type, message_type>, i, producers, &queue);
            producer_threads[i] = thread;
        }
    }

    if (consumer_threads) {
        for (unsigned i = 0; i < consumers; ++i) {
            std::thread * thread = new std::thread(&consumer_thread_proc<queue_type, message_type>, i, consumers, &queue);
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

template <typename QueueType, typename MessageType, size_t InitSize>
void run_test(unsigned producers, unsigned consumers)
{
    typedef QueueType queue_type;
    typedef MessageType message_type;

    using namespace std::chrono;
    time_point<high_resolution_clock> startime = high_resolution_clock::now();

    run_test_threads<queue_type, message_type, InitSize>(producers, consumers);

    time_point<high_resolution_clock> endtime = high_resolution_clock::now();
    duration<double> elapsed_time = duration_cast< duration<double> >(endtime - startime);

    printf("\n");
    printf("elapsed time: %0.3f second(s)\n", elapsed_time.count());
    printf("\n");
}

int main(int argc, char * argv[])
{
    unsigned producers, consumers;
    producers = 2;
    consumers = 2;

    printf("ConcurrentTest.\n");
    printf("\n");
    printf("Messages  = %u\n", kMaxMessageCount);
    printf("Producers = %u\n", producers);
    printf("Consumers = %u\n", consumers);
#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) || defined(__amd64__) || defined(__x86_64__)
    printf("x86_64    = true\n");
#else
    printf("x86_64    = false\n");
#endif
    printf("\n");

    run_test<StdQueueWapper<Message *>, Message, 4096>(producers, consumers);
    run_test<StdDequeueWapper<Message *>, Message, 4096>(producers, consumers);

    run_test<LockedRingQueueWapper<Message *>, Message, 4096>(producers, consumers);
    run_test<FixedLockedRingQueueWapper<Message *>, Message, 4096>(producers, consumers);

    printf("\n");
    ::system("pause");
    return 0;
}
