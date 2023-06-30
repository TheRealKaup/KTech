#include "engine.hpp"

Engine::TimePoint::TimePoint()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}

void Engine::TimePoint::SetToNow()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}

long Engine::TimePoint::Seconds()
{
	return std::chrono::duration_cast<std::chrono::seconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}

long Engine::TimePoint::Milliseconds()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}

long Engine::TimePoint::Microseconds()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}

long Engine::TimePoint::Nanoseconds()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(chronoTimePoint - engineStartTP.chronoTimePoint).count();
}