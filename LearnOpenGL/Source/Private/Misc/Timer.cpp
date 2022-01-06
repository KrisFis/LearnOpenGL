
#include "Timer.h"

namespace NTimerPrivate
{
	FORCEINLINE double ToSeconds(const clock_t& Timepoint)
	{
		// units/(units/time) => time (seconds) * 1000 = milliseconds
		return (Timepoint / (double) CLOCKS_PER_SEC);
	}

	FORCEINLINE bool IsTimepointValid(const clock_t& Timepoint)
	{
		return ToSeconds(Timepoint) > 0.f;
	}
}

FTimer::FTimer()
		: StartTimepoint(), EndTimepoint()
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
	return NTimerPrivate::ToSeconds(EndTimepoint - StartTimepoint) * 10e3;
}

double FTimer::GetSeconds() const
{
	return NTimerPrivate::ToSeconds(EndTimepoint - StartTimepoint);
}

void FTimer::Start()
{
	ENSURE(!NTimerPrivate::IsTimepointValid(StartTimepoint));
	StartTimepoint = std::clock();
}

void FTimer::Stop()
{
	ENSURE(!NTimerPrivate::IsTimepointValid(EndTimepoint));
	EndTimepoint = std::clock();
}

void FTimer::Reset()
{
	StartTimepoint = clock_t();
	EndTimepoint = clock_t();
}
