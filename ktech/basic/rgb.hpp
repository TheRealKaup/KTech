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

//! @brief 24-bit color, able of representing 16,777,216 (2^24) different colors.
struct KTech::RGB
{
	uint8_t r; //!< Red primary color.
	uint8_t g; //!< Green primary color.
	uint8_t b; //!< Blue primary color.

	/*!
		@brief Construct an `RGB` color.
		@param [in] red Red primary color.
		@param [in] green Green primary color.
		@param [in] blue Blue primary color.
	*/
	constexpr RGB(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)
		: r(red), g(green), b(blue) {}

	/*!
		@brief Compare 2 `RGB`s.
		@param [in] rgb The `RGB` to compare with this `RGB`.
		@return `true`: the red, green and blue primary colors are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const RGB& rgb) const -> bool
	{
		return r == rgb.r && g == rgb.g && b == rgb.b;
	}
};