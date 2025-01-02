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
#include "invocation.hpp"

#include <thread>

/*!
	@fn `Time::CallInvocations`

	@brief Call callback functions of finished `Invocation`s.

	Progresses all `Invocation`s by `Time::deltaTime`, and calls those which waited their time.

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
	// ERASE-REMOVE `Invocation`s which were set to `nullptr` by `Time::DeregisterInvocation()` (which was called by `Invocation::~Invocation()`).
	m_invocations.erase(std::remove(m_invocations.begin(), m_invocations.end(), nullptr), m_invocations.end());

	// ADVANCE all invocations:
	// NOLINTNEXTLINE(modernize-loop-convert) // Avoid iterator invalidation (`Invocation::Advance()` can insert into `m_invocations`).
	for (size_t i = 0; i < m_invocations.size(); i++)
	{
		// SKIP invocations deleted meanwhile
		if (m_invocations[i] == nullptr)
		{
			continue;
		}

		if (m_invocations[i]->m_active)
		{
			// ADVANCE invocation:
			m_invocations[i]->m_timePassed += engine.time.deltaTime;

			if (m_invocations[i]->m_timePassed >= m_invocations[i]->m_duration) // Invocation is due.
			{
				// PREVENT from calling again (and INFORM user that the invocation is inactive):
				m_invocations[i]->m_active = false;
				// CALL invoked function:
				if (m_invocations[i]->m_callback())
				{
					// INFORM `Output` to render-on-demand:
					m_changedThisTick = true;
				}
			}
		}
	}
}

/*!
	@fn `Time::WaitUntilNextTick`

	@brief Sleeps and returns when the next tick should start.

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
	// Calculate delta of current tick (`deltaTime`)
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_currentTickStart).count();
	// Calculate `tpsPotential`
	tpsPotential = 1000000.0F / deltaTime;
	// Calculate sleep duration according to `tpsLimit`
	auto sleepDuration = std::chrono::microseconds(1000000 / tpsLimit) - std::chrono::microseconds(deltaTime);
	// Sleep only if needed
	if (sleepDuration.count() > 0)
	{
		std::this_thread::sleep_for(sleepDuration);
	}
	// Calculate (actual) `tps`
	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_currentTickStart).count();
	tps = 1000000.0F / deltaTime;
	// Set `m_currentTickStart` to now
	ticksCounter++;
	m_currentTickStart = std::chrono::steady_clock::now();
}

auto KTech::Time::TimeToMicroseconds(long p_time, Measurement p_measurement) const -> long
{
	switch (p_measurement)
	{
		case Measurement::ticks:
		{
			//     24 -> 24000000 -> 1000000
			return p_time * 1000000 / tpsLimit;
		}
		case Measurement::seconds:
		{
			//     1 -> 1000000
			return p_time * 1000000;
		}
		case Measurement::milliseconds:
		{
			//     1000 -> 1000000
			return p_time * 1000;
		}
		case Measurement::microseconds: default:
		{
			//     1000000
			return p_time;
		}
	}
}

void KTech::Time::RegisterCallback(Invocation* p_invocation)
{
	m_invocations.push_back(p_invocation);
}

void KTech::Time::DeregisterCallback(Invocation* p_invocation)
{
	std::replace(m_invocations.begin(), m_invocations.end(), p_invocation, static_cast<Invocation*>(nullptr));
}