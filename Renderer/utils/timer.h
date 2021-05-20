#pragma once
#include <windows.h>

class Timer
{
public:
	Timer();
	~Timer();

	// In seconds
	float TotalTime() const;
	// In seconds
	float DeltaTime() const;

	// Call before message loop
	void Reset();
	// Call when unpaused
	void Start();
	// Call when paused
	void Stop();
	// Call per frame
	void Tick();

private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_stopped;
};
