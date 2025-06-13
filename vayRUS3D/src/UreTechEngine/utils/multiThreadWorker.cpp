#include "multiThreadWorker.h"

void UreTechEngine::threadWorker(int id, multicoreThreading* parent) {
	SetThreadAffinityMask(GetCurrentThread(), 1 << id);
	HANDLE eventHandle = parent->threadEventHandle[id];
	while (parent->status != DESTROYED) {

		// wait job
		WaitForSingleObject(eventHandle, INFINITE);

		if (parent->threadStatus[id] == PROCESSING) {
			parent->wokerFunc(id, parent->jobs[id].data(), parent->jobs[id].size());
			parent->threadStatus[id] = IDLE; // job done
			ResetEvent(eventHandle); // reset event for next job
		}
	}
}

UreTechEngine::multicoreThreading::multicoreThreading()
{
	threadCount = std::thread::hardware_concurrency();
	threadStatus.resize(threadCount, IDLE);
	threadEventHandle.resize(threadCount, NULL);
	for (int i = 0; i < threadCount; i++) {
		std::thread t(threadWorker, i, this);
		threadEventHandle[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		t.detach();
	}
}

void UreTechEngine::multicoreThreading::addJobs(jobPtr job, size_t jobCount, size_t jobStructSize) {
	splitJobs(job, jobCount, jobStructSize);
}

void UreTechEngine::multicoreThreading::setWorkerFunc(threadWorkFunc func) {
	wokerFunc = func;
}

void UreTechEngine::multicoreThreading::setPreJobFunc(onPreJobFunc func) {
	preJob = func;
}

void UreTechEngine::multicoreThreading::setPostJobFunc(onPostJobFunc func) {
	postJob = func;
}

void UreTechEngine::multicoreThreading::startJobs() {
	if (wokerFunc == nullptr) {
		std::cerr << "Worker function is not set.\n";
		return;
	}
	else {
		// call pre function if set
		if (preJob) preJob();

		// fire all threads
		for (int i = 0; i < threadCount; ++i) {
			threadStatus[i] = PROCESSING;
			workerStatus status = PROCESSING;
			SetEvent(threadEventHandle[i]); // signal thread to start processing
		}
	}

}

void UreTechEngine::multicoreThreading::waitJobsSync() {
	if (status == PROCESSING) {
		while (waitJobsAsync() != 0);
		status = IDLE;
		if (postJob) postJob();
	}
}

int UreTechEngine::multicoreThreading::waitJobsAsync() {
	int stillProcessing = 0;
	for (int i = 0; i < threadCount; ++i) {
		if (threadStatus[i] == PROCESSING) {
			stillProcessing++;
		}
	}

	if (stillProcessing == 0) {
		status = IDLE;
		if (postJob) postJob();
	}

	return stillProcessing;
}

workerStatus UreTechEngine::multicoreThreading::getThreadJobStatus(unsigned int threadID)
{
	return threadStatus[threadID];
}

void UreTechEngine::multicoreThreading::splitJobs(jobPtr job, size_t jobCount, size_t jobStructSize)
{
	int jobsPerThread = jobCount / threadCount;

	std::vector <threadJob> jobref;
	jobs.clear();
	jobs.resize(threadCount, jobref);
	jobref.~vector();
	for (unsigned int i = 0; i < threadCount; ++i) {
		threadJob newJob;
		newJob.job = static_cast<char*>(job) + (i * jobsPerThread * jobStructSize);
		newJob.jobCount = (i == threadCount - 1) ? (jobCount - (i * jobsPerThread)) : jobsPerThread;
		jobs[i].push_back(newJob);
	}
}
