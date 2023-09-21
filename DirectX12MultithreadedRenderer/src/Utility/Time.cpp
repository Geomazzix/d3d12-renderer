#include "Utility/Time.h"
#include <windows.h>
#include "EventSystem/EventMessenger.h"

namespace mtd3d
{
	Time::Time()
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		m_SecondsPerCount = 1.0 / (double)countsPerSec;

		EventMessenger::GetInstance().ConnectListener("OnAppPause", &Time::Stop, this);
		EventMessenger::GetInstance().ConnectListener("OnAppResume", &Time::Start, this);
	}

	Time::~Time()
	{
		EventMessenger::GetInstance().DisconnectListener("OnAppPause", &Time::Stop, this);
		EventMessenger::GetInstance().DisconnectListener("OnAppResume", &Time::Start, this);
	}

	float Time::TotalTime()
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance 
		// mStopTime - mBaseTime includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:  
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

		if (m_Stopped)
		{
			return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
		}

		// The distance mCurrTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from mCurrTime:  
		//
		//  (mCurrTime - mPausedTime) - mBaseTime 
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mCurrTime

		else
		{
			return (float)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
		}
	}

	float Time::DeltaTime()
	{
		return (float)m_DeltaTime;
	}

	void Time::Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_BaseTime = currTime;
		m_PrevTime = currTime;
		m_StopTime = 0;
		m_Stopped = false;
	}

	void Time::Start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


		// Accumulate the time elapsed between stop and start pairs.
		//
		//                     |<-------d------->|
		// ----*---------------*-----------------*------------> time
		//  mBaseTime       mStopTime        startTime     

		if (m_Stopped)
		{
			m_PausedTime += (startTime - m_StopTime);

			m_PrevTime = startTime;
			m_StopTime = 0;
			m_Stopped = false;
		}
	}

	void Time::Stop()
	{
		if (!m_Stopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			m_StopTime = currTime;
			m_Stopped = true;
		}
	}

	void Time::Tick()
	{
		if (m_Stopped)
		{
			m_DeltaTime = 0.0;
			return;
		}

		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_CurrTime = currTime;

		// Time difference between this frame and the previous.
		m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;

		// Prepare for next frame.
		m_PrevTime = m_CurrTime;

		// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
		// processor goes into a power save mode or we get shuffled to another
		// processor, then mDeltaTime can be negative.
		if (m_DeltaTime < 0.0)
		{
			m_DeltaTime = 0.0;
		}
	}

	double Time::m_SecondsPerCount = 0.0;
	double Time::m_DeltaTime = -1.0;

	__int64 Time::m_BaseTime = 0;
	__int64 Time::m_PausedTime = 0;
	__int64 Time::m_StopTime = 0;
	__int64 Time::m_PrevTime = 0;
	__int64 Time::m_CurrTime = 0;

	bool Time::m_Stopped = false;
}