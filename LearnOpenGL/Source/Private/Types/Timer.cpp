
#include "Timer.h"

#include <assert.h>

namespace NTimerPrivate
{
	bool IsTimepointValid(const std::chrono::time_point<std::chrono::high_resolution_clock> Timepoint)
	{
		return Timepoint.time_since_epoch().count() > 0;
	}
}

FTimer::FTimer()
	: StartTimepoint()
	, EndTimepoint()
{}

bool FTimer::IsStarted() const
{
	return NTimerPrivate::IsTimepointValid(StartTimepoint) && !NTimerPrivate::IsTimepointValid(EndTimepoint);
}

bool FTimer::IsFinished() const
{
	return NTimerPrivate::IsTimepointValid(StartTimepoint) && NTimerPrivate::IsTimepointValid(EndTimepoint);
}

double FTimer::GetMilliseconds() const
{
	auto start = std::chrono::time_point_cast<std::chrono::microseconds>(StartTimepoint).time_since_epoch().count();
	auto end = std::chrono::time_point_cast<std::chrono::microseconds>(EndTimepoint).time_since_epoch().count();

	return (end - start) * 10e-3;
}

double FTimer::GetSeconds() const
{
	auto start = std::chrono::time_point_cast<std::chrono::microseconds>(StartTimepoint).time_since_epoch().count();
	auto end = std::chrono::time_point_cast<std::chrono::microseconds>(EndTimepoint).time_since_epoch().count();

	return (end - start) * 10e-6;
}

void FTimer::Start()
{
	assert(!NTimerPrivate::IsTimepointValid(StartTimepoint));
	StartTimepoint = std::chrono::high_resolution_clock::now();
}

void FTimer::Stop()
{
	assert(!NTimerPrivate::IsTimepointValid(EndTimepoint));
	EndTimepoint = std::chrono::high_resolution_clock::now();
}

void FTimer::Reset()
{
	StartTimepoint = std::chrono::time_point<std::chrono::high_resolution_clock>();
	EndTimepoint = std::chrono::time_point<std::chrono::high_resolution_clock>();
}
