#include "/include/threadpool.hpp"

ThreadPool::ThreadPool(size_t numThreads):stop(false){
    worker.reverse(numThreads);
    for(size_t i = 0;i<numThreads;i++){
        workers.emplace_back(&ThreadPool::workLoop,this);
    }
}

