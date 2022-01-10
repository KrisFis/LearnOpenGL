
#include "Timer.h"

FTimer::FTimer()
		: StartTimepoint(), EndTimepoint()
{}

bool FTimer::IsStarted() const
{
	return StartTimepoint > 0.f && EndTimepoint == 0.f;
}

bool FTimer::IsFinished() const
{
	return StartTimepoint > 0.f && EndTimepoint > 0.f;
}

double FTimer::GetMilliseconds() const
{
	return (EndTimepoint - StartTimepoint) * NTimeUtils::MS_PER_SECOND;
}

double FTimer::GetSeconds() const
{
	return EndTimepoint - StartTimepoint;
}

void FTimer::Start()
{
	ENSURE(StartTimepoint == 0.f);
	StartTimepoint = NTimeUtils::GetSecondsSinceEpoch();
}

void FTimer::Stop()
{
	ENSURE(EndTimepoint == 0.f);
	EndTimepoint = NTimeUtils::GetSecondsSinceEpoch();
}

void FTimer::Reset()
{
	StartTimepoint = 0.f;
	EndTimepoint = 0.f;
}
