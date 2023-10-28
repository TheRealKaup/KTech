#include "engine.hpp"

int32_t Engine::Time::deltaTime = 0;
Engine::Time::TimePoint Engine::Time::thisTickStartTP ;
float Engine::Time::potentialfps = 0.0f;
float Engine::Time::fps = 0.0f;
Engine::Time::TimePoint Engine::Time::engineStartTP;
int32_t Engine::Time::totalTicks = 0;

Engine::Time::Invocation* Engine::Time::Invoke(std::function<void()> callback, uint32_t time, Measurement timeMeasurement)
{
	if (timeMeasurement == Measurement::ticks)
		invocations.push_back(new Invocation(callback, time));
	else if (timeMeasurement == Measurement::seconds)
		invocations.push_back(new Invocation(callback, time * tps));
	else
		invocations.push_back(new Invocation(callback, time * tps / 1000));
	return invocations[invocations.size() - 1];
}

bool Engine::Time::CancelInvocation(Invocation* invocation)
{
	for (size_t i = 0; i < invocations.size(); i++)
	{
		if (invocation == invocations[i])
		{
			invocations.erase(invocations.begin() + i);
			delete invocation;
			return true;
		}
	}
	return false;
}


void Engine::Time::CallInvocations()
{
	for (size_t i = 0; i < invocations.size(); i++)
	{
		if (invocations[i]->ticksLeft > 0)
			invocations[i]->ticksLeft--;
		else
		{
			// Call
			if (invocations[i]->callback)
				invocations[i]->callback();
			// Remove invocation
			delete invocations[i];
			invocations.erase(invocations.begin() + i);
			i--;
			continue;
		}
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