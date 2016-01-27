
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

#include <FastQueue/utils/PowOf2.h>
#include <FastQueue/queue/LockedRingQueue.h>

using namespace FastQueue;

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

template <typename T, typename IndexType = uint64_t>
using StdMutexRingQueue = typename LockedRingQueue<T, std::mutex, IndexType>;

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
    Fixed_LockedRingQueue<Message, std::mutex, uint32_t, 511> fixedLockedRingQueue;
    StdMutexRingQueue<Message, uint32_t> stdMutexRingQueue(511);

    Message message;
    int success;

    size_t nSize = fixedLockedRingQueue.sizes();
    size_t nCapacity = fixedLockedRingQueue.capacity();
    printf("fixedLockedRingQueue.sizes() = %d\n", nSize);
    printf("fixedLockedRingQueue.capacity() = %d\n", nCapacity);
    printf("\n");

    message.value = 1;
    success = fixedLockedRingQueue.push_front(std::move(message));
    ASSERT_BOOLEAN_SUCCESS_EX("LockedRingQueue::push_front()", success == OP_STATE_SUCCESS);
    ASSERT_BOOLEAN_PASSED_EX("LockedRingQueue::push_front()", message.value == 1);
    message.value = 2;
    success = fixedLockedRingQueue.pop_front(message);
    ASSERT_BOOLEAN_SUCCESS_EX("LockedRingQueue::pop_front()", success == OP_STATE_SUCCESS);
    ASSERT_BOOLEAN_PASSED_EX("LockedRingQueue::pop_front()", message.value == 1);

    printf("\n");

    nSize = stdMutexRingQueue.sizes();
    nCapacity = stdMutexRingQueue.capacity();
    printf("stdMutexRingQueue.sizes() = %d\n", nSize);
    printf("stdMutexRingQueue.capacity() = %d\n", nCapacity);
    printf("\n");

    message.value = 1;
    success = stdMutexRingQueue.push_front(std::move(message));
    ASSERT_BOOLEAN_SUCCESS_EX("StdMutexRingQueue::push_front()", success == OP_STATE_SUCCESS);
    ASSERT_BOOLEAN_PASSED_EX("StdMutexRingQueue::push_front()", message.value == 1);
    message.value = 2;
    success = stdMutexRingQueue.pop_front(message);
    ASSERT_BOOLEAN_SUCCESS_EX("StdMutexRingQueue::pop_front()", success == OP_STATE_SUCCESS);
    ASSERT_BOOLEAN_PASSED_EX("StdMutexRingQueue::pop_front()", message.value == 1);

    printf("\n");
    
    bool isPow2;
    isPow2 = run_time::is_pow2(0xFFFFFFFFUL);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(0xFFFFFFFFUL)", isPow2 == false);
    isPow2 = run_time::is_pow2(0);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(0)", isPow2 == true);
    isPow2 = run_time::is_pow2(1);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(1)", isPow2 == true);
    isPow2 = run_time::is_pow2(2);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(2)", isPow2 == true);
    isPow2 = run_time::is_pow2(3);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(3)", isPow2 == false);
    isPow2 = run_time::is_pow2(4);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(4)", isPow2 == true);
    isPow2 = run_time::is_pow2(16);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(16)", isPow2 == true);
    isPow2 = run_time::is_pow2(17);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(17)", isPow2 == false);
    isPow2 = run_time::is_pow2(37);
    ASSERT_BOOLEAN_VERIFY_EX("is_pow2(37)", isPow2 == false);

    size_t checkVal;
    checkVal = run_time::verify_pow2(0xFFFFFFFFUL);
    printf("verify_pow2(0xFFFFFFFFUL) : %d\n", checkVal);
    checkVal = run_time::verify_pow2(0);
    printf("verify_pow2(0) : %d\n", checkVal);
    checkVal = run_time::verify_pow2(1);
    printf("verify_pow2(1) : %d\n", checkVal);
    checkVal = run_time::verify_pow2(2);
    printf("verify_pow2(2) : %d\n", checkVal);
    checkVal = run_time::verify_pow2(3);
    printf("verify_pow2(3) : %d\n", checkVal);
    checkVal = run_time::verify_pow2(4);
    printf("verify_pow2(4) : %d\n", checkVal);
    checkVal = run_time::verify_pow2(16);
    printf("verify_pow2(16) : %d\n", checkVal);
    checkVal = run_time::verify_pow2(17);
    printf("verify_pow2(17) : %d\n", checkVal);
    checkVal = run_time::verify_pow2(37);
    printf("verify_pow2(37) : %d\n", checkVal);

    printf("\n");

    std::atomic<int> atomic1;
    int lock_status = 0;
    int new_status = atomic1.compare_exchange_strong(lock_status, 1, std::memory_order_seq_cst);
    printf("new_status = %d\n", new_status);

    printf("\n");
    ::system("pause");
    return 0;
}
