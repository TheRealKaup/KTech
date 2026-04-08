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
	@brief Define KTech::RGB class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION

/*!
	@brief 24-bit color, able of representing 16,777,216 (2^24) different colors.
*/
struct KTech::RGB
{
	uint8_t r; //!< Red primary color.
	uint8_t g; //!< Green primary color.
	uint8_t b; //!< Blue primary color.

	/*!
		@brief Compare 2 `RGB`s.
		@param [in] rgb The `RGB` to compare with this `RGB`.
		@return `true`: the red, green and blue primary colors are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const RGB& rgb) const -> bool = default;
};
