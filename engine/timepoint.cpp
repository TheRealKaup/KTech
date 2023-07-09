#include "engine.hpp"

Engine::Time::TimePoint::TimePoint()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}

void Engine::Time::TimePoint::SetToNow()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}

long Engine::Time::TimePoint::Seconds()
{
	return std::chrono::duration_cast<std::chrono::seconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}

long Engine::Time::TimePoint::Milliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}

long Engine::Time::TimePoint::Microseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}

long Engine::Time::TimePoint::Nanoseconds()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}