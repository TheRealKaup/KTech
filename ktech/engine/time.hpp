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

class KTech::Time
{
public:
	enum class Measurement : uint8_t
	{
		ticks,
		seconds,
		milliseconds,
		microseconds
	};

	struct TimePoint
	{
		std::chrono::high_resolution_clock::time_point chronoTimePoint;
		TimePoint()
			: chronoTimePoint(std::chrono::high_resolution_clock::now()) {}
	};

	struct Invocation
	{
		std::function<void()> callback;
		size_t ticksLeft; // Current ticks left in this instance
		inline Invocation(const std::function<void()>& callback, size_t ticks)
			: callback(callback), ticksLeft(ticks) {}
	};

	int16_t tpsLimit;
	float tps = 0.0f;
	float tpsPotential = 0.0f;
	int32_t deltaTime = 0;
	int32_t ticksCounter = 0;

	inline Time(Engine* const engine, int16_t ticksPerSecondLimit = 24)
		: engine(engine), tpsLimit(ticksPerSecondLimit) {}

	Invocation* Invoke(const std::function<void()>& callback, uint32_t time, Measurement timeMeasurement);
	bool CancelInvocation(Invocation* invocation);
	void CallInvocations();

	size_t GetDelta(const TimePoint& timePointA, const TimePoint& timePointB, Measurement timeMeasurement);
	size_t GetInt(const TimePoint& tp, Measurement timeMeasurement);

	void WaitUntilNextTick();

private:
	Engine* const engine;

	bool invokedThisTick = false;

	const TimePoint m_startTP;
	TimePoint m_thisTickStartTP;
	std::vector<Invocation*> m_invocations;

	friend class KTech::Output;
};