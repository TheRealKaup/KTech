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
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "upoint.hpp"

//! 2D vector, mostly used to store positions and directions.
struct KTech::Point
{
	int32_t x; //!< X axis (+ right, - left).
	int32_t y; //!< Y axis (+ down, - up).

	/*!
		@brief Construct a `Point`.
		@param [in] x X axis.
		@param [in] y Y axis.
	*/
	constexpr Point(int32_t x = 0, int32_t y = 0)
		: x(x), y(y) {}

	/*!
		@brief Construct a `Point` from a `UPoint`.
		@param [in] uPoint Base `UPoint`.
	*/
	constexpr Point(const UPoint& uPoint)
		: x(uPoint.x), y(uPoint.y) {}

	/*!
		@brief Compare 2 `Point`s.
		@param [in] point The `Point` to compare with this `Point`.
		@return `true`: the X and Y axes are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const Point& point) const -> bool
	{
		return (x == point.x) && (y == point.y);
	}

	/*!
		@brief Add 2 `Point`s together.
		@param [in] point The `Point` to add with this `Point`.
		@return New `Point` with the sum of the 2 `Point`s.
	*/
	constexpr auto operator+(const Point& point) const -> Point
	{
		return {x + point.x, y + point.y};
	}

	/*!
		@brief Subtract `Point` from another.
		@param [in] point The `Point` to subtract from this `Point`.
		@return New `Point` with the difference between the 2 `Point`s.
	*/
	constexpr auto operator-(const Point& point) const -> Point
	{
		return {x - point.x, y - point.y};
	}

	/*!
		@brief Add a `Point` to this `Point`.
		@param [in] point The `Point` to add to this `Point`.
		@return Self-reference (for function chaining).
	*/
	constexpr auto operator+=(const Point& point) -> Point&
	{
		x += point.x;
		y += point.y;
		return *this;
	}

	/*!
		@brief Subtract a `Point` from this `Point`.
		@param [in] point The `Point` to subtract from this `Point`.
		@return Self-reference (for function chaining).
	*/
	constexpr auto operator-=(const Point& point) -> Point&
	{
		x -= point.x;
		y -= point.y;
		return *this;
	}
};