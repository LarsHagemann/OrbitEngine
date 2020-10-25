#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <queue>

namespace orbit
{

	// @brief: this threadpool will generate a set amount of threads
	//	and let them handle tasks provided by calls to
	//	ThreadPool::EnqueueTask(...);
	class ThreadPool
	{
	protected:
		using Task = std::function<void()>;
		std::queue<Task> _tasks;
		std::unordered_map<std::thread::id, std::shared_ptr<std::thread>> _threads;
		std::vector<std::thread::id> _availableThreads;
		std::mutex _taskMutex;

		size_t _sleepTime;
		bool _running;
	protected:
		void demon();
	public:
		ThreadPool(unsigned NumThreads = std::thread::hardware_concurrency());
		// @attention: Calling ThreadPool::EnqueueTask(...) is not thread safe!
		template<class _Func, typename... _Args>
		void EnqueueTask(_Func func, _Args... args);
		void ForceStopJoin();
		void ForceStopDetach();
		void AwaitStop();
		void AwaitFinish();

		bool IsValid() const { return _running; }

		void SetSleepTime(size_t sleepTime) { _sleepTime = sleepTime; }
		size_t GetThreadCount() const { return _threads.size(); }
	};

	template<class _Func, typename... _Args>
	void ThreadPool::EnqueueTask(_Func func, _Args... args)
	{
		auto l = [=]() { std::invoke(func, args...); };
		std::lock_guard<std::mutex> lock(_taskMutex);
		_tasks.emplace(l);
	}

}
