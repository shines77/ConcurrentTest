
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <deque>
#include <type_traits>

#include <assert.h>

#define MAX_MESSAGE_COUNT   (100 * 10000)

typedef double  jmc_timestamp_t;

std::mutex g_lock;

namespace ConcourrentTest {

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
    Message(Message && src) {
        this->value = src.value;
    }
    ~Message() {}
};

} // namespace ConcourrentTest

using namespace ConcourrentTest;

template <typename T>
int producer_thread_proc(int thread_idx, T * queue)
{
    printf("Producer Thread: thread_idx = %d, queue = 0x%08X.\n", thread_idx, queue);

    for (int i = 0; i < MAX_MESSAGE_COUNT; ++i) {
        Message * msg = new Message();
        g_lock.lock();
        queue->push_front(msg);
        g_lock.unlock();
    }
    return 0;
}

template <typename T>
int consumer_thread_proc(int thread_idx, T * queue)
{
    printf("Consumer Thread: thread_idx = %d, queue = 0x%08X.\n", thread_idx, queue);

    for (int i = 0; i < MAX_MESSAGE_COUNT; ++i) {
        g_lock.lock();
        Message *& msg = queue->back();
        if (msg) {
            queue->pop_back();
        }
        g_lock.unlock();
    }
    return 0;
}

void run_test_threads(int producers, int consumers)
{
    std::deque<Message *> queue;
    std::thread  ** producer_threads = new std::thread *[producers];
    std::thread  ** consumer_threads = new std::thread *[consumers];

    for (int i = 0; i < producers; ++i) {
        std::thread * thread = new std::thread(&producer_thread_proc<std::deque<Message *>>, i, &queue);
        producer_threads[i] = thread;
    }

    for (int i = 0; i < consumers; ++i) {
        std::thread * thread = new std::thread(&consumer_thread_proc<std::deque<Message *>>, i, &queue);
        consumer_threads[i] = thread;
    }

    for (int i = 0; i < producers; ++i) {
        producer_threads[i]->join();
    }

    for (int i = 0; i < consumers; ++i) {
        consumer_threads[i]->join();
    }

    if (producer_threads) {
        for (int i = 0; i < producers; ++i) {
            delete producer_threads[i];
        }
        delete[] producer_threads;
    }

    if (consumer_threads) {
        for (int i = 0; i < consumers; ++i) {
            delete consumer_threads[i];
        }
        delete[] consumer_threads;
    }
}

int main(int argc, char * argv[])
{
    int thread_num = 4;
    printf("ConcurrentTest.\n");
    printf("\n");

    run_test_threads(4, 4);

    printf("\n");
    ::system("pause");
    return 0;
}
