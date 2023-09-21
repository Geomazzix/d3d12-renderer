#pragma once
#define WIN32_LEAN_AND_MEAN

namespace mtd3d
{
	/// <summary>
	/// The Time class is always existing managing the time from start to end within the application.
	/// Taken over from "3D Game programming with DirectX12 - by Frank D. Luna"
	/// </summary>
	class Time
	{
	public:
		Time();
		~Time();

		// Returns the total time elapsed since Reset() was called, NOT counting any
		// time when the clock is stopped.
		static float TotalTime(); // in seconds
		static float DeltaTime(); // in seconds

		void Reset(); // Call before message loop.
		void Start(); // Call when unpaused.
		void Stop();  // Call when paused.
		void Tick();  // Call every frame.

	private:
		static double m_SecondsPerCount;
		static double m_DeltaTime;

		static __int64 m_BaseTime;
		static __int64 m_PausedTime;
		static __int64 m_StopTime;
		static __int64 m_PrevTime;
		static __int64 m_CurrTime;

		static bool m_Stopped;
	};
}