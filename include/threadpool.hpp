#pragma once

#include<vector>
#include<queue>
#include<mutex>
#include<thread>
#include<condition_variable>
#include <functional>
#include <atomic>

class ThreadPool{
    public:
        explicit ThreadPool(size_t num);
        ~ThreadPool();
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        void submit(std::function<void()>tasks);

    private:
        void workLoop();
    
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>>tasks;

        std::mutex queueMutex;
        std::condition_variable condition;

        std::atomic<bool> stop;
};