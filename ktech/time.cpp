/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 E. Kaufman (AKA Kaup)

	This file is part of KTech.

	KTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	KTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KTech. If not, see <https://www.gnu.org/licenses/>.
*/

#include "ktech.hpp"

int32_t Engine::Time::deltaTime = 0;
float Engine::Time::tpsPotential = 0.0f;
float Engine::Time::tps = 0.0f;

Engine::Time::Invocation* Engine::Time::Invoke(const std::function<void()>& callback, uint32_t time, Measurement timeMeasurement)
{
	switch (timeMeasurement)
	{
		case Measurement::ticks:
		{
			Invocation::invocations.push_back(new Invocation(callback, time));
			break;
		}
		case Measurement::seconds:
		{
			Invocation::invocations.push_back(new Invocation(callback, time * tpsLimit));
			break;
		}
		case Measurement::milliseconds:
		{
			Invocation::invocations.push_back(new Invocation(callback, time * tpsLimit / 1000));
			break;
		}
		case Measurement::microseconds:
		{
			Invocation::invocations.push_back(new Invocation(callback, time * tpsLimit / 1000000));
			break;
		}
		default:
			return nullptr;
	}
	return Invocation::invocations[Invocation::invocations.size() - 1];
}

bool Engine::Time::CancelInvocation(Invocation* invocation)
{
	for (size_t i = 0; i < Invocation::invocations.size(); i++)
	{
		if (invocation == Invocation::invocations[i])
		{
			Invocation::invocations.erase(Invocation::invocations.begin() + i);
			delete invocation;
			return true;
		}
	}
	return false;
}


void Engine::Time::CallInvocations()
{
	for (size_t i = 0; i < Invocation::invocations.size(); i++)
	{
		if (Invocation::invocations[i]->ticksLeft > 0)
			Invocation::invocations[i]->ticksLeft--;
		else
		{
			// Call
			if (Invocation::invocations[i]->callback)
				Invocation::invocations[i]->callback();
			// Remove invocation
			delete Invocation::invocations[i];
			Invocation::invocations.erase(Invocation::invocations.begin() + i);
			i--;
			continue;
		}
	}
}

void Engine::Time::WaitUntilNextTick()
{
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::startTP.chronoTimePoint).count() - thisTickStartTP.GetInt(Measurement::microseconds);
	tpsPotential = 1000000.0f / deltaTime;
	std::this_thread::sleep_for(std::chrono::microseconds(1000000 / tpsLimit - deltaTime));
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - Engine::Time::startTP.chronoTimePoint).count() - thisTickStartTP.GetInt(Measurement::microseconds);
	tps = 1000000.0f / deltaTime;
	thisTickStartTP.SetToNow();
	ticksCounter++;
}