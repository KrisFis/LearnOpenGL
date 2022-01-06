
#include "Timer.h"

namespace NTimerPrivate
{
	FORCEINLINE double ToMilliseconds(const clock_t& Timepoint)
	{
		// units/(units/time) => time (seconds) * 1000 = milliseconds
		return (Timepoint / (double) CLOCKS_PER_SEC) * 10e3;
	}

	FORCEINLINE bool IsTimepointValid(const clock_t& Timepoint)
	{
		return ToMilliseconds(Timepoint) > 0;
	}
	
	FORCEINLINE double ElapsedTimeInMiliseconds(const clock_t& Start, const clock_t& End)
	{
		return ToMilliseconds(End) - ToMilliseconds(Start);
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
	return NTimerPrivate::ElapsedTimeInMiliseconds(StartTimepoint, EndTimepoint);
}

double FTimer::GetSeconds() const
{
	return NTimerPrivate::ElapsedTimeInMiliseconds(StartTimepoint, EndTimepoint) * 10e-3;
}

void FTimer::Start()
{
	ENSURE(!NTimerPrivate::IsTimepointValid(StartTimepoint));
	StartTimepoint = clock();
}

void FTimer::Stop()
{
	ENSURE(!NTimerPrivate::IsTimepointValid(EndTimepoint));
	EndTimepoint = clock();
}

void FTimer::Reset()
{
	StartTimepoint = clock_t();
	EndTimepoint = clock_t();
}
