#pragma once
#include <functional>
#include <thread>
#include <queue>
#include <condition_variable>
class CustomTerminateThread : std::exception
{
public:
	CustomTerminateThread();
	~CustomTerminateThread();
};
class ThreadPool
{
public:
	ThreadPool(int p_ThreadCount);
	~ThreadPool();

	std::mutex lock;
	std::condition_variable m_ConditionVariable;
	std::queue<std::function<void()>> m_Work;
	std::vector<std::thread> m_Workers;

	std::function<void()> GetNextWorkItem();
	void StopThreads();
	void DoWork(int id);
	void AddWork(const std::function<void()>& p_Function);

private:
	bool m_Stopping = false;
	bool m_Stop = false;
	bool m_Finished = false;
};

