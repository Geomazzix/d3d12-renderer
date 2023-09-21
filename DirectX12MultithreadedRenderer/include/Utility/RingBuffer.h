#pragma once
#include <cstdint>
#include <mutex>

namespace mtd3d
{
	/// <summary>
	/// Thread safe implementation of a very simple ring buffer.
	/// </summary>
	template<typename T, size_t Capacity>
	class RingBuffer
	{
	public:
		RingBuffer();
		~RingBuffer() = default;

		bool PushBack(const T& item);
		bool PopFront(T& item);

		size_t GetCapacity() const;

	private:
		size_t m_Head;
		size_t m_Tail;
		T m_Data[Capacity];

		std::mutex m_RingBufferMutex;
	};


	template<typename T, size_t Capacity>
	mtd3d::RingBuffer<T, Capacity>::RingBuffer() :
		m_Head(0),
		m_Tail(0)
	{}

	template<typename T, size_t Capacity>
	bool mtd3d::RingBuffer<T, Capacity>::PopFront(T& item)
	{
		bool result = false;
		m_RingBufferMutex.lock();

		if(m_Tail != m_Head)
		{
			m_Data[m_Tail] = nullptr;
			m_Tail = (m_Tail + 1) % Capacity;
		}

		m_RingBufferMutex.unlock();
		return result;
	}

	template<typename T, size_t Capacity>
	bool mtd3d::RingBuffer<T, Capacity>::PushBack(const T& item)
	{
		bool result = false;
		m_RingBufferMutex.lock();

		size_t next = (m_Head + 1) % Capacity;
		if (next >= m_Tail)
		{
			m_Data[m_Head] = item;
			m_Head = next;
			result = true;
		}

		m_RingBufferMutex.unlock();
		return result;
	}

	template<typename T, size_t Capacity>
	size_t mtd3d::RingBuffer<T, Capacity>::GetCapacity() const
	{
		return Capacity;
	}
}