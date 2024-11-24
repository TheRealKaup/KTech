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
#include "upoint.hpp"

struct KTech::Point
{
	int32_t x, y;

	constexpr Point(int32_t x = 0, int32_t y = 0)
		: x(x), y(y) {}

	constexpr Point(const UPoint& uPoint)
		: x(uPoint.x), y(uPoint.y) {}

	constexpr auto operator==(const Point& point) const -> bool
	{
		return (x == point.x) && (y == point.y);
	}

	constexpr auto operator!=(const Point& point) const -> bool
	{
		return !(*this == point);
	}

	constexpr auto operator+(const Point& point) const -> Point
	{
		return {x + point.x, y + point.y};
	}

	constexpr auto operator-(const Point& point) const -> Point
	{
		return {x - point.x, y - point.y};
	}

	constexpr auto operator+=(const Point& point) -> Point&
	{
		x += point.x;
		y += point.y;
		return *this;
	}

	constexpr auto operator-=(const Point& point) -> Point&
	{
		x -= point.x;
		y -= point.y;
		return *this;
	}
};