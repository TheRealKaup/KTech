#include "engine.hpp"

Engine::Time::TimePoint::TimePoint()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}

void Engine::Time::TimePoint::SetToNow()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}

long Engine::Time::TimePoint::GetInt(Measurement timeMeasurement) const
{
	switch (timeMeasurement)
	{
		case Measurement::ticks:
		{
			return std::chrono::duration_cast<std::chrono::seconds>(chronoTimePoint - startTP.chronoTimePoint).count() * Engine::running;
		}
		case Measurement::seconds:
		{
			return std::chrono::duration_cast<std::chrono::seconds>(chronoTimePoint - startTP.chronoTimePoint).count();
		}
		case Measurement::milliseconds:
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(chronoTimePoint - startTP.chronoTimePoint).count();
		}
		case Measurement::microseconds:
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(chronoTimePoint - startTP.chronoTimePoint).count();
		}
		default:
			return 0;
	}
}