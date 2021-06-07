#include "Engine/Misc/ThreadPool.hpp"
#include "Engine/Bindings/Platform.hpp"
#include "Engine/Misc/Helper.hpp"

namespace orbit
{

	ThreadPool::ThreadPool(unsigned NumThreads) :
		_running(true),
		_sleepTime(1u)
	{
		_threads.reserve(NumThreads);
		for (auto i = 0u; i < NumThreads; ++i)
		{
			auto thread = std::make_shared<std::thread>(&ThreadPool::demon, this);
			pt::SetThreadName(*thread.get(), FormatString(L"TP Demon: %d", i));
			_threads.emplace(thread->get_id(), thread);
			_availableThreads.emplace_back(thread->get_id());
		}
	}

	void ThreadPool::demon()
	{
		while (_running)
		{
			const auto my_id = std::this_thread::get_id();
			Task task;
			{
				while (!_taskMutex.try_lock()) { pt::Sleep(_sleepTime); }
				std::lock_guard<std::mutex> lock(_taskMutex, std::adopt_lock);

				// look for a task given to this thread
				if (!_tasks.empty())
				{
					task = _tasks.front();
					auto it = std::find(_availableThreads.begin(), _availableThreads.end(), my_id);
					_availableThreads.erase(it);
					_tasks.pop();
				}
			}
			if (task)
			{
				task();
				_availableThreads.emplace_back(my_id);
			}
			else pt::Sleep(_sleepTime);
		}
	}

	void ThreadPool::ForceStopJoin()
	{
		_running = false;
		for (auto& thread : _threads)
			if (thread.second->joinable())
				thread.second->join();
	}

	void ThreadPool::ForceStopDetach()
	{
		_running = false;
		for (auto& thread : _threads)
			thread.second->detach();
	}

	void ThreadPool::AwaitFinish()
	{
		while (_availableThreads.size() != _threads.size() || !_tasks.empty()) 
			pt::Sleep(_sleepTime);
	}

	void ThreadPool::AwaitStop()
	{
		while (!_tasks.empty()) 
			pt::Sleep(_sleepTime);

		_running = false;
		for (auto& thread : _threads)
			if (thread.second->joinable())
				thread.second->join();
	}

}