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
	@brief Define KTech::CellA class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "rgba.hpp"

/*!
	@brief Like `Cell`, but with `RGBA` foreground and background colors, instead of `RGB`.
*/
struct KTech::CellA
{
	RGBA b; //!< Background color.
	char c; //!< ASCII character.
	RGBA f; //!< Foreground (character) color.

	/*!
		@brief Compare 2 `CellA`s.
		@param [in] cellA The `CellA` to compare with this `CellA`.
		@return `true`: the foreground colors, background colors, and
	   characters are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const CellA& cellA) const -> bool = default;
};
