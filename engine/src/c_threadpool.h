#pragma once
#include <thread>
#include <condition_variable>
#include <queue>
#include <memory>

class CompletionEvent
{
public:
	std::mutex m_mutex;
	void waitForCompletion(int completionCount = 1);
	void markComplete();

private:
	unsigned int m_times = 0;
	std::condition_variable m_cv;
};

struct Job
{
	std::function<void(std::shared_ptr<void>)> callback;
	std::shared_ptr<void> callbackArguments = nullptr;
	CompletionEvent* completionEvent = nullptr;
};

class ThreadPool
{
public:
	ThreadPool();
	int m_threadCount;
	void initialize(int threadCount);
	void deinitialize();
	void submitJob(std::function<void(std::shared_ptr<void>)> callback, std::shared_ptr<void> callbackArguments, CompletionEvent* completionEvent);
	static void jobCheck(ThreadPool* threadPool);

private:
	std::vector<std::shared_ptr<std::thread>> m_threads;
	std::queue<Job> m_jobs;
	std::mutex m_jobsMutex;
	bool m_running;
};