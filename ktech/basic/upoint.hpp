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

struct KTech::UPoint
{
	uint32_t x, y;

	inline constexpr UPoint(uint32_t x = 0, uint32_t y = 0)
		: x(x), y(y) {}

	inline constexpr bool operator==(const UPoint& point) const
	{
		return (x == point.x) && (y == point.y);
	}

	inline constexpr bool operator!=(const UPoint& point) const
	{
		return !(*this == point);
	}

	inline constexpr UPoint operator+(const UPoint& point) const
	{
		return UPoint(x + point.x, y + point.y);
	}

	inline constexpr UPoint operator-(const UPoint& point) const
	{
		return UPoint(x - point.x, y - point.y);
	}

	inline constexpr UPoint& operator+=(const UPoint& point)
	{
		x += point.x;
		y += point.x;
		return *this;
	}

	inline constexpr UPoint& operator-=(const UPoint& point)
	{
		x -= point.x;
		y -= point.x;
		return *this;
	}
};