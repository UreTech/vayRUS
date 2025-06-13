#pragma once
#include <UreTechEngine/EngineCore.h>
#include <iostream>
#include <vector>
#include <thread>
#include <windows.h>

namespace UreTechEngine
{

	typedef void* jobPtr;

	class multicoreThreading; // Forward declaration

	void ENGINE_DEFINE threadWorker(int id, multicoreThreading* parent);

	typedef void (*threadWorkFunc)(int workerID, jobPtr job, size_t jobCount);
	typedef void (*onPreJobFunc)();
	typedef void (*onPostJobFunc)();

	typedef enum {
		IDLE = 0x00,
		DESTROYED = 0xFF,
		PROCESSING = 0x1A,
	}workerStatus;


	struct threadJob
	{
		jobPtr job = nullptr;
		size_t jobCount = 0;
		jobPtr jobResult;
		size_t jobResultSize;
	};

	class ENGINE_DEFINE multicoreThreading
	{
		friend void UreTechEngine::threadWorker(int id, multicoreThreading* parent);
	public:
		multicoreThreading();

		void addJobs(jobPtr job, size_t jobCount, size_t jobStructSize);

		void setWorkerFunc(threadWorkFunc func);

		void setPreJobFunc(onPreJobFunc func);

		void setPostJobFunc(onPostJobFunc func);

		void startJobs();

		void waitJobsSync();

		int waitJobsAsync();

		workerStatus getStatus() const { return status; }

		workerStatus getThreadJobStatus(unsigned int threadID);

		int getThreadCount() const { return threadCount; }
	private:
		void splitJobs(jobPtr job, size_t jobCount, size_t jobStructSize);

		std::vector<std::vector<threadJob>> jobs;
		unsigned int threadCount = std::thread::hardware_concurrency();

		std::vector<workerStatus> threadStatus;
		std::vector<HANDLE> threadEventHandle;
		workerStatus status = IDLE;

		threadWorkFunc wokerFunc = nullptr;
		onPreJobFunc preJob = nullptr;
		onPostJobFunc postJob = nullptr;
	};

}