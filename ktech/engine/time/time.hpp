/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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

#pragma once

#define KTECH_DEFINITION
#include "../../ktech.hpp"
#undef KTECH_DEFINITION

#include <chrono>

/*!
	@brief Engine component responsible for game loop timing.

	This engine component schedules ticks (game loop iterations). Its member variables, which hold various information on tick timing, are at your disposal. `Time` also manages your `Invocation`s.
*/
class KTech::Time
{
public:
	//! @brief Time measurement.
	enum class Measurement : uint8_t
	{
		ticks,
		seconds,
		milliseconds,
		microseconds
	};

	struct Invocation;

	unsigned long tpsLimit; //!< Max ticks allowed to occur in a second. You set this value in `Engine::Engine()`, and you can change it whenever you want.
	float tps = 0; //!< Actual ticks per second. Corresponds to `Time::deltaTime`.
	float tpsPotential = 0; //!< Ticks per second if it wasn't limited by `Time::tpsLimit`.
	long deltaTime = 0; //!< Duration of the last tick, in microseconds.
	unsigned long ticksCounter = 0; //!< Total ticks since game started.

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
	void CallInvocations();

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
	void WaitUntilNextTick();

private:
	bool m_changedThisTick = false;
	Engine& engine;
	std::chrono::steady_clock::time_point m_currentTickStart;
	std::vector<Invocation*> m_invocations;

	Time(Engine& engine, unsigned long ticksPerSecondLimit)
		: engine(engine), tpsLimit(ticksPerSecondLimit) {}

	[[nodiscard]] auto TimeToMicroseconds(long p_time, Measurement p_measurement) const -> long;
	void RegisterCallback(Invocation* invocation);
	void DeregisterCallback(Invocation* invocation);

	friend class Invocation;
	friend class Output;
	friend class Engine;
};
