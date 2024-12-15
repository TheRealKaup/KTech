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

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION

#include <chrono>
#include <functional>

/*!
	Engine component responsible for game loop timing.

	This engine component schedules ticks (game loop iterations). Its member variables, which hold various information on tick timing, are at your disposal. `Time` can also invoke your callback functions given a delay.
*/
class KTech::Time
{
public:
	//! Time measurement.
	enum class Measurement : uint8_t
	{
		ticks,
		seconds,
		milliseconds
	};

	//!	Class containing information on an invocation. @see `Time::Invoke`
	class Invocation
	{
	private:
		std::function<bool()> callback;
		size_t ticksLeft; // Current ticks left in this instance
		inline Invocation(const std::function<bool()>& callback, size_t ticks)
			: callback(callback), ticksLeft(ticks) {}
		friend Time;
	};

	size_t tpsLimit; //!< Max ticks allowed to occur in a second. You set this value in `Engine::Engine()`, and you can change it whenever you want.
	float tps; //!< Actual ticks per second. Corresponds to `Time::deltaTime`.
	float tpsPotential; //!< Ticks per second if it wasn't limited by `Time::tpsLimit`.
	long deltaTime; //!< Duration of the last tick, in microseconds.
	size_t ticksCounter = 0; //!< Total ticks since game started.

	auto Invoke(const std::function<bool()>& callback, size_t time, Measurement timeMeasurement) -> Invocation*;
	auto CancelInvocation(Invocation* invocation) -> bool;
	void CallInvocations();

	void WaitUntilNextTick();

private:
	bool m_changedThisTick = false;
	Engine& engine;
	std::chrono::high_resolution_clock::time_point m_currentTickStart;
	std::vector<Invocation*> m_invocations;

	inline Time(Engine& engine, size_t ticksPerSecondLimit)
		: engine(engine), tpsLimit(ticksPerSecondLimit) {}

	friend class Output;
	friend class Engine;
};