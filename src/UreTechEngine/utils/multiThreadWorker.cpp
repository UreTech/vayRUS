#include "multiThreadWorker.h"

void UreTechEngine::threadWorker(int id, multicoreThreading* parent) {
	SetThreadAffinityMask(GetCurrentThread(), (1ULL << id));
	SetPriorityClass(GetCurrentThread(), HIGH_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	string name = ("Thread Worker " + u64ToDecStr(id));
	std::wstring wname(name.data(), name.data() + name.lenght());
	SetThreadDescription(GetCurrentThread(), wname.c_str());
	//HANDLE eventHandle = parent->threadEventHandle[id];

	while (parent->status != DESTROYED) {

		// wait job
		WaitForSingleObject(parent->semaphore, INFINITE);

		timer ttimer;
		ttimer.start();

		parent->preJob(id);

		// check if there are jobs exists to process
		if (!parent->jobs[id].empty()) {

			parent->wokerFunc(id, parent->jobs[id].data(), parent->jobs[id].size());


		}
		parent->postJob(id);

		if (parent->workersRemaining.fetch_sub(1, std::memory_order_release) == 1) {
			SetEvent(parent->doneEventHandle);
		}

		//EngineConsole::log("thread " + u64ToDecStr(id) + " took " + u64ToDecStr(ttimer.get_elapsed_ms()) + "ms to end.", EngineConsole::INFO);

		//EngineConsole::log("thread " + u64ToDecStr(id) + " exit " + u64ToDecStr(--parent->workersRemaining) + " thread remaining.", EngineConsole::INFO);

	}
}

UreTechEngine::multicoreThreading::multicoreThreading()
{
	threadCount = std::thread::hardware_concurrency();
	
	workersRemaining.store(0);

	semaphore = CreateSemaphore(nullptr, 0, threadCount, nullptr);
	doneEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	for (int i = 0; i < threadCount; i++) {
		std::thread t(threadWorker, i, this);
		HANDLE nativeHandle = t.native_handle();
		DWORD threadId = GetThreadId(nativeHandle);
		threadsIDs.push_back(threadId);
		t.detach();
	}
}

void UreTechEngine::multicoreThreading::addJobs(jobPtr job, size_t jobCount, size_t jobStructSize) {
	splitJobs(job, jobCount, jobStructSize);
}

void UreTechEngine::multicoreThreading::setWorkerFunc(threadWorkFunc func) {
	wokerFunc = func;
}

void UreTechEngine::multicoreThreading::setPreWorkersFunc(onPreWorkersFunc func) {
	preWorkers = func;
}

void UreTechEngine::multicoreThreading::setPostWorkersFunc(onPostWorkersFunc func) {
	postWorkers = func;
}

void UreTechEngine::multicoreThreading::setPreJobFunc(onPreJobFunc func) {
	preJob = func;
}

void UreTechEngine::multicoreThreading::setPostJobFunc(onPostJobFunc func) {
	postJob = func;
}

void UreTechEngine::multicoreThreading::startJobs() {
	if (wokerFunc == nullptr) {
		EngineConsole::log("Worker function is not set.", EngineConsole::LOG_ERROR);
		return;
	}
	else {
		// call pre function
		 preWorkers();

		status = PROCESSING;

		workersRemaining.store(threadCount);

		// fire all threads
		ReleaseSemaphore(semaphore, threadCount, nullptr);
	}

}

void UreTechEngine::multicoreThreading::waitJobsSync() {
	//timer ttimer;
	//ttimer.start();

	WaitForSingleObject(doneEventHandle, INFINITE);

	//	if (ttimer.get_elapsed_ms() > 100) {
	//		EngineConsole::log("This frame took very long time " + u64ToDecStr(ttimer.get_elapsed_ms()) + "ms elapsed.", EngineConsole::WARN);
	//	}

	postWorkers();
	status = IDLE;

	//EngineConsole::log("thread jobs took " + u64ToDecStr(ttimer.get_elapsed_ms()) + "ms to end.", EngineConsole::INFO);
}

int UreTechEngine::multicoreThreading::waitJobsAsync() {
	//EngineConsole::log(u64ToDecStr(workersRemaining) + " thread remaining.", EngineConsole::INFO);
	return workersRemaining;
}

unsigned int UreTechEngine::multicoreThreading::getThreadID(unsigned int threadIndex)
{
	return threadsIDs[threadIndex];
}

void UreTechEngine::multicoreThreading::splitJobs(jobPtr job, size_t jobCount, size_t jobStructSize)
{
	jobs.clear();
	jobs.resize(threadCount);

	if (!jobCount) return;

	for (size_t i = 0; i < jobCount; i++) {
		threadJob newJob;
		newJob.job = *(jobPtr*)((char*)job + (i * jobStructSize));
		jobs[i % threadCount].push_back(newJob);
	}
}
