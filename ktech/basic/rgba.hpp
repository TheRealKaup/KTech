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
	@brief Define KTech::RGBA class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#include <cstdint>
#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "rgb.hpp"

/*!
	@brief Like `RGB`, but also has an alpha channel representing transparency.
*/
struct KTech::RGBA
{
	uint8_t r; //!< Red primary color.
	uint8_t g; //!< Green primary color.
	uint8_t b; //!< Blue primary color.
	uint8_t a; //!< Alpha channel.

	/*!
		@brief Compare 2 `RGBA`s.
		@param [in] rgba The `RGBA` to compare with this `RGBA`.
		@return `true`: the alpha channel, red, green and blue primary colors are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const RGBA& rgba) const -> bool = default;

	/*!
		@brief Narrow an RGBA into an RGB.
		@return KTech::RGB whose red, green and blue values are equal to those of the original RGBA (the alpha channel is discarded).
	*/
	explicit constexpr operator RGB() const
	{
		return RGB{.r = r, .g = g, .b = b};
	}
};

/*!
	@brief Widen an RGB into an RGBA.

	@param [in] rgb RGB to widen.
	@param [in] alpha Alpha channel to add to the RGB
	@return KTech::RGBA whose red, green and blue values are equal to those of the original RGB, and alpha channel is equal to the given alpha value.
*/
auto constexpr operator|(const KTech::RGB& rgb, uint8_t alpha) -> KTech::RGBA
{
	return {.r = rgb.r, .g = rgb.g, .b = rgb.b, .a = alpha};
}
