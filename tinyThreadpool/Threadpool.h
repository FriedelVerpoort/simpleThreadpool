#pragma once
#include <thread>
#include <vector>
#include <mutex>
#include <queue>

namespace simpleThreadpool
{
	template<typename ...args>
	struct ThreadFunctionContainer
	{
		std::function<void(args...)> func;
		std::tuple<args...> arg;

		explicit ThreadFunctionContainer(std::function<void(args...)> f, args... argList) :
			func{f},
			arg{std::tuple<args...>(std::forward<args>(argList)...)}
		{		
		}
	};

	class IThreadFunction
	{
	public:
		IThreadFunction() = default;
		virtual ~IThreadFunction() = default;

		virtual void operator()() = 0;
	};

	template<typename  ...args>
	class ThreadFunction : public IThreadFunction
	{
	public:
		ThreadFunction(std::function<void(args...)> f, args... argList)
			:m_Container(f, std::forward<args>(argList)...)
		{
		}
		
		template<std::size_t...Is>
		void CallFunction(std::function<void(args...)>& func, const std::tuple<args...>& tuple, std::index_sequence<Is...>)
		{
			func(std::get<Is>(tuple)...);
		}

		virtual void operator()() override
		{
			CallFunction(m_Container.func, m_Container.arg, std::index_sequence_for<args...>());
		}
	private:
		ThreadFunctionContainer<args...> m_Container;
	};

	class Threadpool
	{
		
	public:
		Threadpool(size_t threads);
		~Threadpool();
		void WorkingLoop();
		void AddJob(IThreadFunction* Job);

		void AddJobs(std::initializer_list<IThreadFunction*> list);
		static int GetMaxThreads();
		bool AllJoinable();
		void AllJoin();

		int TotalJoinable() const;
		int TotalThreads() const;

		void SetStop(bool stop) { m_Stop = stop; m_Conditional.notify_all(); };
	private:
		static int m_MaxThreads;

		std::vector<std::thread> m_Threads;
		std::queue<IThreadFunction*> m_FunctionQueue;
		std::mutex m_Lock;
		std::condition_variable m_Conditional;
		bool m_Stop{ false };
	};
};