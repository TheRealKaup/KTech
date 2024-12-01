/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2024 Ethan Kaufman (AKA Kaup)

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

#include "time.hpp"

#include <thread>

/*!
	@fn `Time::Invoke`

	Invoke a callback function after a given delay.

	Adds an `Invocation` to the list, and when time is due, your function will be called.

	Actual calling of your function is done in `Time::CallInvocations()`, meaning, the delay is synchronized with your game loop's ticks per second (TPS) rate. So, unless you are running an extremely fast game loop, you are likely not going to see a difference between 1 `Time::Measurement::ticks` and 1 `Time::Measurement::milliseconds`.

	@param [in] callback Your callback function which returns `bool`. The return value is explained in `Output::ShouldRenderThisTick()`.
	@param [in] time Delay duration.
	@param [in] timeMeasurement The given `time`'s measurement .

	@return Pointer to the `Invocation` created. The only thing you can do with it is pass it to `CancelInvocation()`, if you wish to cancel the invocation.

	@see `Time::Measurement`
	@see `Time::CallInvocations()`
	@see `Output::ShouldRenderThisTick()`
*/
auto KTech::Time::Invoke(const std::function<bool()>& p_callback, size_t p_time, Measurement p_timeMeasurement) -> Invocation*
{
	switch (p_timeMeasurement)
	{
		case Measurement::ticks:
		{
			m_invocations.push_back(new Invocation(p_callback, p_time));
			break;
		}
		case Measurement::seconds:
		{
			m_invocations.push_back(new Invocation(p_callback, p_time * tpsLimit));
			break;
		}
		case Measurement::milliseconds:
		{
			m_invocations.push_back(new Invocation(p_callback, p_time * tpsLimit / 1000));
			break;
		}
		default:
			return nullptr;
	}
	return m_invocations[m_invocations.size() - 1];
}

/*!
	@fn `Time::CancelInvocation`

	Cancel an awaiting invocation.

	@param [in] invocation The `Time::Invocation` pointer you got from `Time::Invoke()` when you initially invoked it.

	@return True if the given `invocation` was cancelled and deleted, false if it doesn't exist.
*/
auto KTech::Time::CancelInvocation(Invocation* p_invocation) -> bool
{
	for (size_t i = 0; i < m_invocations.size(); i++)
	{
		if (p_invocation == m_invocations[i])
		{
			m_invocations.erase(m_invocations.begin() + i);
			delete p_invocation;
			return true;
		}
	}
	return false;
}

/*!
	@fn `Time::CallInvocations`

	Call callback functions of due invocations.

	Progresses all invocations by 1 tick, and calls those which have no ticks left in their delay.

	Normally placed at the start of your game loop, among the other callback-calling functions. For example:

	@code{.cpp}
	// Game loop
	while (engine.running)
	{
		// Call various callback-functions
		engine.input.CallCallbacks();
		engine.time.CallInvocations(); // <- Call due invocations.
		engine.memory.CallOnTicks();

		// Graphics...
	}
	@endcode

	@see `Time::Invoke()`
*/
void KTech::Time::CallInvocations()
{
	for (size_t i = 0; i < m_invocations.size(); i++)
	{
		if (m_invocations[i]->ticksLeft > 0)
		{
			m_invocations[i]->ticksLeft--;
		}
		else
		{
			// Call
			if (m_invocations[i]->callback)
			{
				if (m_invocations[i]->callback())
				{
					m_changedThisTick = true;
				}
			}
			// Remove invocation
			delete m_invocations[i];
			m_invocations.erase(m_invocations.begin() + i);
			i--;
			continue;
		}
	}
}

/*!
	@fn `Time::WaitUntilNextTick`

	Sleeps and returns when the next tick should start.

	Calculates how long to sleep (based on how long the current tick is, and `Time::tpsLimit`). It enters sleep and returns when the following tick should occur. This function also updates `Time::tpsPotential`, `Time::deltaTime`, `Time::tps`, and `Time::ticksCounter`.

	Normally placed at the end of your game loop:

	@code{.cpp}
	// Game loop
	while (engine.running)
	{
		// Call various callback-functions...
		// Graphics...
		engine.time.WaitUntilNextTick();
	}
	@endcode

	@see `Time::tpsLimit`
	@see `Time::tpsPotential`
	@see `Time::deltaTime`
	@see `Time::tps`
	@see `Time::ticksCounter`
*/
void KTech::Time::WaitUntilNextTick()
{
	// Calculate `tpsPotential`
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_currentTickStart).count();
	tpsPotential = 1000000.0F / deltaTime;
	// Sleep according to `tpsLimit`
	std::this_thread::sleep_for(std::chrono::microseconds(1000000 / tpsLimit - deltaTime));
	// Calculate (actual) `tps`
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_currentTickStart).count();
	tps = 1000000.0F / deltaTime;
	m_currentTickStart = std::chrono::high_resolution_clock::now();
	ticksCounter++;
}