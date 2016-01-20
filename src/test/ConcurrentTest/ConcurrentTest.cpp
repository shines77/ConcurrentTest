
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

namespace ConcourrentTest {

enum benchmark_type_t {
    Boost_lockfree_queue,
    Intel_TBB_concurrent_queue,
    MoodyCamel_ConcurrentQueue,
    CXX11_Disruptor,

    // Max test queue type
    kMaxQueueType
};

} // namespace ConcourrentTest

using namespace ConcourrentTest;

int main(int argc, char * argv[])
{
    printf("ConcurrentTest.\n");
    ::system("pause");
    return 0;
}
