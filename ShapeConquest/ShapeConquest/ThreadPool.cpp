#include "ThreadPool.h"
#include <ostream>
#include <iostream>


ThreadPool::ThreadPool(int p_ThreadCount)
{
	m_Workers.reserve(p_ThreadCount);

	for (int i = 0; i < p_ThreadCount; ++i)
	{
		m_Workers.emplace_back(&ThreadPool::DoWork, this, i);
	}
}

ThreadPool::~ThreadPool()
{
	StopThreads();
	m_ConditionVariable.notify_all();
	for (std::thread& thread : m_Workers)
	{
		thread.join();
	}
}

std::function<void()> ThreadPool::GetNextWorkItem()
{
	std::unique_lock<std::mutex> locker(lock);

	//m_ConditionVariable.wait(locker, [this]() {return !this->m_Work.empty(); });
	while (!m_Stopping &&  m_Work.empty())
		m_ConditionVariable.wait(locker);
	if (locker && !m_Work.empty())
	{
		std::function<void()> function = m_Work.front();
		m_Work.pop();
		return function;
	}
	return nullptr;

}

void ThreadPool::StopThreads()
{
	std::unique_lock<std::mutex> locker(lock, std::try_to_lock_t());
	if (locker)
	{
		m_Stopping = true;
		std::queue<std::function<void()>> emptyQueue;
		std::swap(m_Work, emptyQueue);
		for (auto &thread : m_Workers)
		{
			//thread.join();
			m_Work.push([]() {throw CustomTerminateThread(); });
		}
	}
}

void ThreadPool::DoWork(int id)
{
	while (true)
	{
		try
		{
			auto item = GetNextWorkItem();
			if (item)
			{
				//std::cout << "Thread " << id << " completing task" << std::endl;
				item();
			}
		}
		catch (CustomTerminateThread const& e) {
			// Trigger break out of while loop
			// Only triggered when thread is terminated
			// Can't be triggered for any other reason
			break;
		}
		catch (std::exception &e) {
			// TODO:L:Add logging here?
			std::cout << "Error: " << e.what() << std::endl;
		}
	}
}

void ThreadPool::AddWork(const std::function<void()>& p_Function)
{
	std::unique_lock<std::mutex> locker(lock);

	if (m_Stopping)
	{
		// No work can be added
		//throw std::runtime_error("Thread pool shutting down, no further work can be added");
	}
	
	m_Work.push(p_Function);
	m_ConditionVariable.notify_one();
}

CustomTerminateThread::CustomTerminateThread()
{

}

CustomTerminateThread::~CustomTerminateThread()
{

}
