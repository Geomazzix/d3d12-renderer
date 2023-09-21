#include "JobSystem.h"

#pragma warning (push)
#pragma warning (disable : 26451)

namespace mtd3d
{

	JobSystem::JobSystem() : 
		m_JobValue(0)
	{}

	void JobSystem::Initialize()
	{
		m_FinishedValue.store(0);
		uint32_t numThreads = std::max(1u, std::thread::hardware_concurrency());

		for(uint32_t i = 0; i < numThreads; ++i)
		{
			std::thread worker(&JobSystem::WorkerThread, this);
			worker.detach();
		}
	}

	void JobSystem::Execute(const ThreadJob& job)
	{
		m_JobValue++;

		//Keep checking if a job can be added, if not keep notifying threads that there are unfinished jobs, 
		//in order to prevent them from falling asleep.
		while(!m_JobPool.PushBack(job))
		{
			Poll();
		}

		m_WakeCondition.notify_one();
	}

	bool JobSystem::IsBusy()
	{
		return m_FinishedValue.load() > m_JobValue;
	}

	void JobSystem::Wait()
	{
		while(IsBusy())
		{
			Poll();
		}
	}

	void JobSystem::Poll()
	{
		m_WakeCondition.notify_one();
		std::this_thread::yield();		//Allow the the thread to be rescheduled.
	}

	void JobSystem::WorkerThread()
	{
		ThreadJob job;

		while(true)
		{
			if(m_JobPool.PopFront(job))
			{
				job();
				m_FinishedValue.fetch_add(1);
			}
			else
			{
				std::unique_lock<std::mutex> lock(m_LockMutex);
				m_WakeCondition.wait(lock);
			}
		}
	}
}

#pragma warning (pop)