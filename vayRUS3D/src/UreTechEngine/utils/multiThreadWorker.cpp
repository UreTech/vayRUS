#include "multiThreadWorker.h"

ThreadPool::ThreadPool(size_t threadCount) {
    for (size_t i = 0; i < threadCount; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers) {
        worker.detach();
    }
}

void ThreadPool::enqueueJob(std::function<void()> job) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobQueue.push(std::move(job));
        activeJobs++;
    }
    condition.notify_one();
}

void ThreadPool::waitForJobs() {
    std::unique_lock<std::mutex> lock(queueMutex);
    condition.wait(lock, [this] { return jobQueue.empty() && activeJobs == 0; });
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !jobQueue.empty(); });

            if (stop && jobQueue.empty()) return;
            job = std::move(jobQueue.front());
            jobQueue.pop();
        }
        job();
        activeJobs--;
        condition.notify_all();
    }
}

