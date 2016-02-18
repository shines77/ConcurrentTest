
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

#include "Messages.h"

#include "MapMessageFactory.h"
#include "UnorderedMapMessageFactory.h"
#include "VectorMessageFactory.h"
#include "ArrayMessageFactory.h"

#if defined(NDEBUG)
#define MAX_ITERATIONS      (8 * 1000 * 10000)
#else
#define MAX_ITERATIONS      (20 * 10000)
#endif

#define DEFAULT_SEED_VALUE  (20160218)
#define TEST_VECTOR_SIZE    (16384)

#define CXX11_DEFAULT_METHOD_DECLEAR    1
#define CXX11_DELETE_METHOD_DECLEAR     1

#if defined(CXX11_DEFAULT_METHOD_DECLEAR) && (CXX11_DEFAULT_METHOD_DECLEAR != 0)
#define JIMI_DEFAULT_METHOD     = default
#else
#define JIMI_DEFAULT_METHOD     {}
#endif

#if defined(CXX11_DELETE_METHOD_DECLEAR) && (CXX11_DELETE_METHOD_DECLEAR != 0)
#define JIMI_DELETE_METHOD      = delete
#else
#define JIMI_DELETE_METHOD      {}
#endif

typedef double  jmc_timestamp_t;

enum {
    kMaxIterations = MAX_ITERATIONS
};

namespace local {

template <typename T, bool HasLocked = false>
class scoped_lock {
private:
    typedef T mutex_type;
    mutex_type & mutex_;

    scoped_lock(mutex_type const &) JIMI_DELETE_METHOD;
    scoped_lock & operator = (mutex_type const &) JIMI_DELETE_METHOD;

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
#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0403)
    ::InitializeCriticalSectionAndSpinCount(&mutex_, 1);
#else
    ::InitializeCriticalSection(&mutex_);
#endif
  }

  ~Mutex() {
    ::DeleteCriticalSection(&mutex_);
  }

  void lock() {
    ::EnterCriticalSection(&mutex_);
  }

  bool try_lock() {
#if defined(_WIN32_WINNT) && (_WIN32_WINNT > 0x0400)
    return (::TryEnterCriticalSection(&mutex_) != 0);
#else
    return false;
#endif
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
    return (pthread_mutex_trylock(&mutex_) != 0);
  }

  void unlock() {
    pthread_mutex_unlock(&mutex_);
  }
};
#else // Other OS
#error WTF
#endif // _WIN32

} // namespace local

static
inline int get_random_int32() {
#if defined(RAND_MAX) && (RAND_MAX == 0x7FFF)
    return ((rand() & 0x07FFF) << 30 | (rand() & 0x07FFF) << 15 | (rand() & 0x07FFF)) & 0x7FFFFFFFUL;
#else
    return (rand() & 0x7FFFFFFFUL);
#endif
}

int get_a_range_random(int range_min, int range_max) {
    if (range_min < range_max) {
        return (range_min + get_random_int32() % (range_max - range_min));
    }
    else if (range_min > range_max) {
        return (range_max + get_random_int32() % (range_min - range_max));
    }
    return range_min;
}

void generate_random_messages(std::vector<unsigned int> &message_vec, unsigned max_size, unsigned int seed) {
    ::srand(seed);
    message_vec.clear();
    message_vec.resize(max_size);
    for (unsigned i = 0; i < max_size; ++i) {
        message_vec[i] = get_a_range_random(CEPH_MSG_FIRST, CEPH_MSG_LAST - 1);
    }
}

enum {
    test_id_first = 0,
    test_id_ceph_switch_case = test_id_first,
    test_id_calc_sum_only,
    test_id_map_message_factory,
    test_id_unordered_map_message_factory,
    test_id_vector_message_factory,
    test_id_array_message_factory,
    test_id_last
};

const char * getTestTypeName(size_t test_type)
{
    switch (test_type) {
        case test_id_ceph_switch_case:
            return "test_id_ceph_switch_case";
        case test_id_calc_sum_only:
            return "test_id_calc_sum_only";
        case test_id_map_message_factory:
            return "test_id_map_message_factory";
        case test_id_unordered_map_message_factory:
            return "test_id_unordered_map_message_factory";
        case test_id_vector_message_factory:
            return "test_id_vector_message_factory";
        case test_id_array_message_factory:
            return "test_id_array_message_factory";
        default:
            return "unknown_test_id_name";
    }
}

template <size_t test_type>
Message * create_a_message(unsigned int msg_id) {
    // Do nothing!!
    return nullptr;
}

template <>
Message * create_a_message<test_id_ceph_switch_case>(unsigned int msg_id) {
    Message * m = nullptr;
    unsigned int type = msg_id;
    switch (type) {
    case CEPH_MSG_ID_0:
        m = new CephMessage00();
        break;
    case CEPH_MSG_ID_1:
        m = new CephMessage01();
        break;
    case CEPH_MSG_ID_2:
        m = new CephMessage02();
        break;
    case CEPH_MSG_ID_3:
        m = new CephMessage03();
        break;
    case CEPH_MSG_ID_4:
        m = new CephMessage04();
        break;
    case CEPH_MSG_ID_5:
        m = new CephMessage05();
        break;
    case CEPH_MSG_ID_6:
        m = new CephMessage06();
        break;
    case CEPH_MSG_ID_7:
        m = new CephMessage07();
        break;
    case CEPH_MSG_ID_8:
        m = new CephMessage08();
        break;
    case CEPH_MSG_ID_9:
        m = new CephMessage09();
        break;
    case CEPH_MSG_ID_10:
        m = new CephMessage10();
        break;
    case CEPH_MSG_ID_11:
        m = new CephMessage11();
        break;
    case CEPH_MSG_ID_12:
        m = new CephMessage12();
        break;
    case CEPH_MSG_ID_13:
        m = new CephMessage13();
        break;
    case CEPH_MSG_ID_14:
        m = new CephMessage14();
        break;
    case CEPH_MSG_ID_15:
        m = new CephMessage15();
        break;
    case CEPH_MSG_ID_16:
        m = new CephMessage16();
        break;
    case CEPH_MSG_ID_17:
        m = new CephMessage17();
        break;
    case CEPH_MSG_ID_18:
        m = new CephMessage18();
        break;
    case CEPH_MSG_ID_19:
        m = new CephMessage19();
        break;
    case CEPH_MSG_ID_20:
        m = new CephMessage20();
        break;
    case CEPH_MSG_ID_21:
        m = new CephMessage21();
        break;
    default:
        break;
    }
    if (m == nullptr) {
        // Unknown message id
    }
    return m;
}

template <>
Message * create_a_message<test_id_calc_sum_only>(unsigned int msg_id) {
    unsigned int type = msg_id;
    Message * m = MapMessageFactory::get().createMessage(type);
    if (m == nullptr) {
        // Unknown message id
    }
    return m;
}

template <>
inline
Message * create_a_message<test_id_map_message_factory>(unsigned int msg_id) {
    unsigned int type = msg_id;
    Message * m = MapMessageFactory::get().createMessage(type);
    if (m == nullptr) {
        // Unknown message id
    }
    return m;
}

template <>
inline
Message * create_a_message<test_id_unordered_map_message_factory>(unsigned int msg_id) {
    unsigned int type = msg_id;
    Message * m = UnorderedMapMessageFactory::get().createMessage(type);
    if (m == nullptr) {
        // Unknown message id
    }
    return m;
}

template <>
inline
Message * create_a_message<test_id_vector_message_factory>(unsigned int msg_id) {
    unsigned int type = msg_id;
    Message * m = VectorMessageFactory::get().createMessage(type);
    if (m == nullptr) {
        // Unknown message id
    }
    return m;
}

template <>
inline
Message * create_a_message<test_id_array_message_factory>(unsigned int msg_id) {
    unsigned int type = msg_id;
    Message * m = VectorMessageFactory::get().createMessage(type);
    if (m == nullptr) {
        // Unknown message id
    }
    return m;
}

template <size_t test_type>
void switch_case_test_thread_proc(unsigned thread_idx, unsigned nthreads, unsigned iterations)
{
#if 0
    // Get a unknown test_id
    printf("Unknown test_type = %zu\n", test_type);
#else
    static const unsigned int seed = DEFAULT_SEED_VALUE;
    static const unsigned int vec_size = TEST_VECTOR_SIZE;

    std::vector<unsigned int> message_vec;
    uint64_t sum = 0;
    static unsigned null_cnt = 0;
    unsigned loop_times = (iterations / vec_size) + 1;
    generate_random_messages(message_vec, vec_size, seed);
    for (unsigned i = 0; i < loop_times; ++i) {
        for (std::vector<unsigned int>::const_iterator it = message_vec.begin(); it != message_vec.end(); ++it) {
            Message * message = create_a_message<test_type>(*it);
            if (message == nullptr) {
                null_cnt++;
            }
            else {
#if 1
                sum += message->getType();
                delete message;
#else
                unsigned int msg_type = message->getType();
                if (msg_type >= CEPH_MSG_ID_VALID_FIRST && msg_type <= CEPH_MSG_ID_VALID_LAST) {
                    sum += message->getType();
                }
                else {
                    printf("test_type = %zu, msg_type = %u\n", test_type, msg_type);
                    Sleep(1);
                }
#endif
            }
        }
    }
    printf("Total messages:   %u\n"
           "Null  messages:   %u\n"
           "Sum of MsgType:   %llu\n"
           "\n",
           loop_times * vec_size, null_cnt, sum);
#endif
}

template <>
void switch_case_test_thread_proc<test_id_calc_sum_only>(unsigned thread_idx, unsigned nthreads, unsigned iterations)
{
    static const unsigned int seed = DEFAULT_SEED_VALUE;
    static const unsigned int vec_size = TEST_VECTOR_SIZE;

    std::vector<unsigned int> message_vec;
    uint64_t sum = 0;
    static unsigned null_cnt = 0;
    unsigned loop_times = (iterations / vec_size) + 1;
    generate_random_messages(message_vec, vec_size, seed);
    for (unsigned i = 0; i < loop_times; ++i) {
        for (std::vector<unsigned int>::const_iterator it = message_vec.begin(); it != message_vec.end(); ++it) {
            unsigned int msg_id = *it;
            if (msg_id >= CEPH_MSG_ID_VALID_FIRST && msg_id <= CEPH_MSG_ID_VALID_LAST) {
                sum += msg_id;
            }
            else {
                null_cnt++;
            }
        }
    }
    printf("Total messages:   %u\n"
           "Null  messages:   %u\n"
           "Sum of MsgType:   %llu\n"
           "\n",
           loop_times * vec_size, null_cnt, sum);
}

template <size_t test_type>
void run_switch_case_test_threads(unsigned nthreads, unsigned total_iterations)
{
    std::thread ** test_threads = new std::thread *[nthreads];

    unsigned iterations = total_iterations / nthreads;
    if (test_threads) {
        for (unsigned i = 0; i < nthreads; ++i) {
            std::thread * thread = new std::thread(switch_case_test_thread_proc<test_type>,
                i, nthreads, iterations);
            test_threads[i] = thread;
        }
    }

    if (test_threads) {
        for (unsigned i = 0; i < nthreads; ++i) {
            if (test_threads[i])
                test_threads[i]->join();
        }
    }

    if (test_threads) {
        for (unsigned i = 0; i < nthreads; ++i) {
            if (test_threads[i])
                delete test_threads[i];
        }
        delete[] test_threads;
    }
}

template <size_t test_type = 0>
void run_switch_case_test(unsigned nthreads, unsigned iterations)
{
    printf("Test for: run_switch_case_test<%s>()\n", getTestTypeName(test_type));
    printf("\n");
    printf("Threads    = %u\n", nthreads);
    printf("Iterations = %u\n", iterations);
    printf("\n");

    using namespace std::chrono;
    time_point<high_resolution_clock> startime = high_resolution_clock::now();

    run_switch_case_test_threads<test_type>(nthreads, iterations);

    time_point<high_resolution_clock> endtime = high_resolution_clock::now();
    duration<double> elapsed_time = duration_cast< duration<double> >(endtime - startime);

    //printf("\n");
    printf("Elapsed time: %0.3f second(s)\n", elapsed_time.count());
    printf("\n");
}

int main(int argc, char * argv[])
{
    unsigned iterations, nthreads;
    iterations = kMaxIterations;

    printf("SwitchCaseTest.\n");
    printf("\n");
#if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_IA64) \
 || defined(__amd64__) || defined(__x86_64__)
    printf("x86_64    = true\n");
#else
    printf("x86_64    = false\n");
#endif
    printf("\n");

#if 1
    printf("-------------------------------------------------------------------------\n");

    nthreads = 1;
    //run_switch_case_test<test_id_calc_sum_only>(nthreads, iterations);
    run_switch_case_test<test_id_ceph_switch_case>(nthreads, iterations);
    run_switch_case_test<test_id_vector_message_factory>(nthreads, iterations);
    run_switch_case_test<test_id_array_message_factory>(nthreads, iterations);
    run_switch_case_test<test_id_map_message_factory>(nthreads, iterations);
    run_switch_case_test<test_id_unordered_map_message_factory>(nthreads, iterations);
    

#if 0
    printf("-------------------------------------------------------------------------\n");

    nthreads = 2;
    //run_switch_case_test<test_id_calc_sum_only>(nthreads, iterations);
    run_switch_case_test<test_id_ceph_switch_case>(nthreads, iterations);
    run_switch_case_test<test_id_vector_message_factory>(nthreads, iterations);
    run_switch_case_test<test_id_array_message_factory>(nthreads, iterations);
    run_switch_case_test<test_id_map_message_factory>(nthreads, iterations);
    run_switch_case_test<test_id_unordered_map_message_factory>(nthreads, iterations);
#endif

    printf("-------------------------------------------------------------------------\n");
#endif

    printf("\n");
    ::system("pause");
    return 0;
}
