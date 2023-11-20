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

Engine::Time::TimePoint::TimePoint()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}

void Engine::Time::TimePoint::SetToNow()
{
	chronoTimePoint = std::chrono::high_resolution_clock::now();
}

long Engine::Time::TimePoint::GetInt(Measurement timeMeasurement) const
{
	switch (timeMeasurement)
	{
		case Measurement::ticks:
		{
			return std::chrono::duration_cast<std::chrono::seconds>(chronoTimePoint - startTP.chronoTimePoint).count() * Engine::running;
		}
		case Measurement::seconds:
		{
			return std::chrono::duration_cast<std::chrono::seconds>(chronoTimePoint - startTP.chronoTimePoint).count();
		}
		case Measurement::milliseconds:
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(chronoTimePoint - startTP.chronoTimePoint).count();
		}
		case Measurement::microseconds:
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(chronoTimePoint - startTP.chronoTimePoint).count();
		}
		default:
			return 0;
	}
}