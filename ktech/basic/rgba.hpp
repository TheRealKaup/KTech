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
#include "rgb.hpp"

//! @brief Like `RGB`, but also has an alpha channel representing transparency.
struct KTech::RGBA : RGB
{
	uint8_t a; //!< Alpha channel.

	/*!
		@brief Construct an `RGBA` color.
		@param [in] red Red primary color.
		@param [in] green Green primary color.
		@param [in] blue Blue primary color.
		@param [in] alpha Alpha channel.
	*/
	constexpr RGBA(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0)
		: RGB(red, green, blue), a(alpha) {}

	/*!
		@brief Construct an `RGBA` color from an `RGB` color.
		@param [in] rgb `RGB` base.
		@param [in] alpha Alpha channel.
	*/
	constexpr RGBA(RGB rgb, uint8_t alpha)
		: RGB(rgb), a(alpha) {}

	/*!
		@brief Compare 2 `RGBA`s.
		@param [in] rgba The `RGBA` to compare with this `RGBA`.
		@return `true`: the alpha channel, red, green and blue primary colors are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const RGBA& rgba) const -> bool
	{
		return (r == rgba.r) && (g == rgba.g) && (b == rgba.b) && (a == rgba.a);
	}
};