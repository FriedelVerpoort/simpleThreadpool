#include "stdafx.h"
#include "Threadpool.h"
#include <iostream>
using namespace simpleThreadpool;

int Threadpool::m_MaxThreads = std::thread::hardware_concurrency();

Threadpool::Threadpool(size_t threads)
{
	m_Threads.reserve(threads);
	for(size_t i{}; i < threads; i++)
	{
		m_Threads.push_back(std::thread(&Threadpool::WorkingLoop, this));
	}
	m_Threads.shrink_to_fit();
}

Threadpool::~Threadpool()
{
	SetStop(true);
	for (std::thread& thread : m_Threads)
	{
		if (thread.joinable())
			thread.join();
	}
}

void Threadpool::WorkingLoop()
{
	while(true)
	{
		IThreadFunction* func = nullptr;
		{
			std::unique_lock<std::mutex> lock(m_Lock);
			m_Conditional.wait(lock, [&]() {return !m_FunctionQueue.empty() || m_Stop; });
			if (m_Stop)
				return;
			func = m_FunctionQueue.front();
			m_FunctionQueue.pop();
		}
		(*func)();
	}
}

void Threadpool::AddJob(IThreadFunction* Job)
{
	{
		std::unique_lock <std::mutex> lock(m_Lock);
		m_FunctionQueue.push(Job);
	}
	m_Conditional.notify_one();
}

void Threadpool::AddJobs(std::initializer_list<IThreadFunction*> list)
{
	{
		std::unique_lock<std::mutex> lock(m_Lock);
		for (auto elem : list)
		{
			m_FunctionQueue.push(elem);
		}
	}

	for (size_t i{}; i < list.size(); i++)
		m_Conditional.notify_one();
}

int Threadpool::GetMaxThreads()
{
	return m_MaxThreads;
}

bool Threadpool::AllJoinable()
{
	for (size_t i{}; i < m_Threads.size(); i++)
	{
		if (!m_Threads[i].joinable() || !m_Stop)
			return false;
	}
	return true;
}

void Threadpool::AllJoin()
{
	for (size_t i{}; i < m_Threads.size(); i++)
		m_Threads[i].join();
}

int Threadpool::TotalJoinable() const
{
	int i{};
	for (size_t i{}; i < m_Threads.size(); i++)
	{
		if (m_Threads[i].joinable())
			i++;
	}
	return i;
}

int Threadpool::TotalThreads() const
{
	return m_Threads.size();
}