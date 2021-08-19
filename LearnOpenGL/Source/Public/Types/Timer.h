
#pragma once

#include <chrono>

class FTimer
{

private:

	typedef std::chrono::time_point<std::chrono::high_resolution_clock> TClock;

public:

	FTimer();

public:

	bool IsStarted() const;
	bool IsFinished() const;

public:

	double GetMilliseconds() const;
	double GetSeconds() const;

public:

	void Start();
	void Stop();
	void Reset();

private:

	TClock StartTimepoint;
	TClock EndTimepoint;
};