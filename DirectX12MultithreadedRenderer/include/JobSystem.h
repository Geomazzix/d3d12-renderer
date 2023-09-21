#pragma once
#include <cstdint>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "Utility/RingBuffer.h"

namespace mtd3d
{
	/// <summary>
	/// The job system manages multiple worker threads equal to the amount available on the GPU.
	/// Implemented using this article on the threading system of the wicked engine as base - the goal of this renderer is not
	/// to experiment with threading, but experiment with threading on directX12 and the impact it can have on the performance.
	/// https://wickedengine.net/2018/11/24/simple-job-system-using-standard-c/
	/// </summary>
	class JobSystem
	{
		using ThreadJob = std::function<void()>;

	public:
		JobSystem();
		~JobSystem() = default;

		void Initialize();
		void Execute(const ThreadJob& job);
		
		bool IsBusy();
		void Wait();
		
	private:
		RingBuffer<ThreadJob, 256> m_JobPool;
		uint64_t m_JobValue;
		std::atomic_uint64_t m_FinishedValue;
		std::mutex m_LockMutex;
		std::condition_variable m_WakeCondition;
	
		void Poll();
		void WorkerThread();
	};
}