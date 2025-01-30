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

//! @brief Unsigned 2D vector, mostly used to store sizes and 2D indexes.
struct KTech::UPoint
{
	uint32_t x; //!< X axis.
	uint32_t y; //!< Y axis.

	/*!
		@brief Construct a `UPoint`.
		@param [in] x X axis.
		@param [in] y Y axis.
	*/
	constexpr UPoint(uint32_t x = 0, uint32_t y = 0)
		: x(x), y(y) {}

	/*!
		@brief Compare 2 `UPoint`s.
		@param [in] uPoint The `UPoint` to compare with this `UPoint`.
		@return `true`: the X and Y axes are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const UPoint& uPoint) const -> bool
	{
		return (x == uPoint.x) && (y == uPoint.y);
	}

	/*!
		@brief Add 2 `UPoint`s together.
		@param [in] uPoint The `UPoint` to add with this `UPoint`.
		@return New `UPoint` with the sum of the 2 `UPoint`s.
	*/
	constexpr auto operator+(const UPoint& uPoint) const -> UPoint
	{
		return {x + uPoint.x, y + uPoint.y};
	}

	/*!
		@brief Subtract `UPoint` from another.
		@param [in] uPoint The `UPoint` to subtract from this `UPoint`.
		@return New `UPoint` with the difference between the 2 `UPoint`s.
	*/
	constexpr auto operator-(const UPoint& uPoint) const -> UPoint
	{
		return {x - uPoint.x, y - uPoint.y};
	}

	/*!
		@brief Add a `UPoint` to this `UPoint`.
		@param [in] uPoint The `UPoint` to add to this `UPoint`.
		@return Self-reference (for function chaining).
	*/
	constexpr auto operator+=(const UPoint& uPoint) -> UPoint&
	{
		x += uPoint.x;
		y += uPoint.y;
		return *this;
	}

	/*!
		@brief Subtract a `UPoint` from this `UPoint`.
		@param [in] uPoint The `UPoint` to subtract from this `UPoint`.
		@return Self-reference (for function chaining).
	*/
	constexpr auto operator-=(const UPoint& uPoint) -> UPoint&
	{
		x -= uPoint.x;
		y -= uPoint.y;
		return *this;
	}
};