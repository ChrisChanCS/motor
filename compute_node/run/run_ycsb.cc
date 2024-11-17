
#include "handler/handler.h"

// Entrance to run threads that spawn coroutines as coordinators to run distributed transactions
int main(int argc, char *argv[])
{
    if (argc != 9)
    {
        std::cerr << "./run_ycsb <thread_num> <coroutine_num> <access_pattern> <skewness> <readWriteRatio> <readOnlyTransaction> <crossPartitionProbability> <isolation_level>" << std::endl;
        return 0;
    }

    Handler *handler = new Handler();

    handler->ConfigureComputeNodeForYCSB(argc, argv);

    handler->GenThreads("ycsb");

    handler->OutputResult("ycsb", "Motor");
}
