
#pragma once

#include "ModuleMinimal.h"

#include <ctime>

class FTimer
{
	
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

	clock_t StartTimepoint;
	clock_t EndTimepoint;
};