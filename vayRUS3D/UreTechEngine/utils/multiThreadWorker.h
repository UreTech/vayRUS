#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>
#include <functional>

class ThreadPool {
public:
    ThreadPool(){}
    ThreadPool(size_t threadCount);
    ~ThreadPool();

    void enqueueJob(std::function<void()> job);
    void waitForJobs();

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> jobQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<size_t> activeJobs = 0;
    bool stop = false;

    void workerThread();

};