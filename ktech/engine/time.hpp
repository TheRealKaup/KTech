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

// Time manager
class KTech::Time
{
public:
	const bool& running;
	
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
		void SetToNow();
		TimePoint();
	};

	// Get the amount of time that has passed since `TimePoint a` until `TimePoint b`.
	long GetDelta(const TimePoint& a, const TimePoint& b, Measurement timeMeasurement);
	// Get the amount of time that has passed since the creation of `Time` and `TimePoint tp`.
	long GetInt(const TimePoint& tp, Measurement timeMeasurement);

	const TimePoint startTP;
	TimePoint thisTickStartTP;

	int16_t tpsLimit = 24;
	float tps = 0.0f;
	float tpsPotential = 0.0f;
	int32_t deltaTime = 0;
	int32_t ticksCounter = 0;

	inline void StartThisTick() { thisTickStartTP.SetToNow(); }
	void WaitUntilNextTick();
	
	struct Invocation
	{
		inline static std::vector<Invocation*> invocations;
		std::function<void()> callback;
		uint32_t ticksLeft; // Current ticks left in this instance
		inline Invocation(const std::function<void()>& callback, uint32_t ticks)
			: callback(callback), ticksLeft(ticks) {}
	};
	Invocation* Invoke(const std::function<void()>& callback, uint32_t time, Measurement timeMeasurement);
	bool CancelInvocation(Invocation* invocation);
	void CallInvocations();

	inline Time(int16_t tps, const bool& running) : tps(tps), running(running) {}
};