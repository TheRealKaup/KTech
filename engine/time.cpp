#include "engine.hpp"

int32_t Engine::Time::deltaTime = 0;
Engine::Time::TimePoint Engine::Time::thisTickStartTP ;
float Engine::Time::potentialfps = 0.0f;
float Engine::Time::fps = 0.0f;
Engine::Time::TimePoint Engine::Time::engineStartTP;
int32_t Engine::Time::totalTicks = 0;

void Engine::Time::Invoke(std::function<void()> callback, uint32_t time, Measurement timeMeasurement, uint32_t instances)
{
	if (timeMeasurement == Measurement::ticks)
		invocations.push_back(Invocation(callback, time, instances));
	else if (timeMeasurement == Measurement::seconds)
		invocations.push_back(Invocation(callback, time * tps, instances));
	else
		invocations.push_back(Invocation(callback, time * tps / 1000, instances));
}

void Engine::Time::CallInvocations()
{
	for (size_t i = 0; i < invocations.size(); i++)
	{
		// Check at the start if already should call (invocations can requested with no delay)
		if (invocations[i].ticksLeft == 0)
		{
			// Call
			if (invocations[i].callback)
				invocations[i].callback();
			
			if (invocations[i].instancesLeft > 1)
			{
				// More instances left, proceed to the next one.
				invocations[i].instancesLeft--;
				invocations[i].ticksLeft = invocations[i].ticks;
			}
			else
			{
				// No instances left, remove invocation from vector.
				invocations.erase(invocations.begin() + i);
				i--;
				continue;
			}
		}

		if (invocations[i].ticksLeft != 0)
			invocations[i].ticksLeft--;
	}
}

void Engine::Time::WaitUntilNextTick()
{
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count() - thisTickStartTP.Microseconds();
	potentialfps = 1000000.0f / deltaTime;
	std::this_thread::sleep_for(std::chrono::microseconds(1000000 / tps - deltaTime));
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::engineStartTP.chronoTimePoint).count() - thisTickStartTP.Microseconds();
	fps = 1000000.0f / deltaTime;
	thisTickStartTP.SetToNow();
	totalTicks++;
}