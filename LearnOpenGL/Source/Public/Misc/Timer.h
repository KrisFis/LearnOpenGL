
#pragma once

#include "ModuleMinimal.h"

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

	double StartTimepoint;
	double EndTimepoint;
};