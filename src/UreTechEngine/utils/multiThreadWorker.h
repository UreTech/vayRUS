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
	typedef void (*onPreWorkersFunc)();
	typedef void (*onPostWorkersFunc)();

	typedef void (*onPreJobFunc)(int workerID);
	typedef void (*onPostJobFunc)(int workerID);

	typedef enum {
		IDLE = 0x00,
		DESTROYED = 0xFF,
		PROCESSING = 0x1A,
	}workerStatus;


	struct threadJob
	{
		jobPtr job = nullptr;
	};

	class ENGINE_DEFINE multicoreThreading
	{
		friend void UreTechEngine::threadWorker(int id, multicoreThreading* parent);
	public:
		multicoreThreading();

		void addJobs(jobPtr job, size_t jobCount, size_t jobStructSize);

		void setWorkerFunc(threadWorkFunc func);

		void setPreWorkersFunc(onPreWorkersFunc func);

		void setPostWorkersFunc(onPostWorkersFunc func);

		void setPreJobFunc(onPreJobFunc func);

		void setPostJobFunc(onPostJobFunc func);

		void startJobs();

		void waitJobsSync();

		int waitJobsAsync();

		workerStatus getStatus() const { return status; }

		workerStatus getThreadJobStatus(unsigned int threadIndex);

		unsigned int getThreadID(unsigned int threadIndex);

		int getThreadCount() const { return threadCount; }
	private:
		bool postCalled = true;
		void splitJobs(jobPtr job, size_t jobCount, size_t jobStructSize);

		dArray<dArray<threadJob>> jobs; // job lists per thread
		unsigned int threadCount = std::thread::hardware_concurrency();

		alignas(64) std::atomic<int> workersRemaining{ 0 };
		HANDLE doneEventHandle = NULL;
		HANDLE semaphore = NULL;
		dArray<unsigned int> threadsIDs;
		workerStatus status = IDLE;

		threadWorkFunc wokerFunc = nullptr;
		onPreWorkersFunc preWorkers = nullptr;
		onPostWorkersFunc postWorkers = nullptr;
		onPreJobFunc preJob = nullptr;
		onPostJobFunc postJob = nullptr;
	};

}