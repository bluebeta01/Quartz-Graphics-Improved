#include "pch.h"
#include "c_threadpool.h"

void CompletionEvent::waitForCompletion(int completionCount)
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cv.wait(lock, [&] { return m_times == completionCount; });
}

void CompletionEvent::markComplete()
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_times++;
	}
	m_cv.notify_all();
}



ThreadPool::ThreadPool() : m_threadCount(0), m_running(true)
{

}

void ThreadPool::submitJob(std::function<void(std::shared_ptr<void>)> callback, std::shared_ptr<void> callbackArguments, CompletionEvent* completionEvent)
{
	Job job = { callback, callbackArguments, completionEvent };
	std::lock_guard<std::mutex> lock(m_jobsMutex);
	m_jobs.push(job);
}

void ThreadPool::initialize(int threadCount)
{
	m_threadCount = std::max(threadCount, 1);
	for (int i = 0; i < m_threadCount; i++)
	{
		std::shared_ptr<std::thread> thread = std::make_shared<std::thread>(ThreadPool::jobCheck, this);
		m_threads.push_back(thread);
	}
}

void ThreadPool::deinitialize()
{
	m_running = false;
	for (std::shared_ptr<std::thread> thread : m_threads)
		thread->join();
}

void ThreadPool::jobCheck(ThreadPool* threadPool)
{
	Job job;
	while (threadPool->m_running)
	{
		{
			std::lock_guard<std::mutex> lock(threadPool->m_jobsMutex);
			if (threadPool->m_jobs.size() == 0)
			{
				std::this_thread::yield();
				//std::this_thread::sleep_for(std::chrono::microseconds(1));
				continue;
			}
			job = threadPool->m_jobs.front();
			threadPool->m_jobs.pop();
		}

		job.callback(job.callbackArguments);
		if(job.completionEvent)
			job.completionEvent->markComplete();
	}
}