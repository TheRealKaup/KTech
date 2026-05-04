/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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

/*!
	@file
	@brief Define KTech::Time members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "time.hpp"

#include <thread>

void KTech::Time::WaitUntilNextTick()
{
	// Calculate delta of current tick (`deltaTime`)
	deltaTime =
		std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_currentTickStart)
			.count();
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
	deltaTime =
		std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_currentTickStart)
			.count();
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
	case Measurement::microseconds:
	default:
	{
		//     1000000
		return p_time;
	}
	}
}
