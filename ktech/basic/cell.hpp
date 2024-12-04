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
#include "rgb.hpp"
#include "../utility/rgbcolors.hpp"

//! Terminal cell comprising `RGB` foreground (character) color, `RGB` background color, and an ASCII character.
struct KTech::Cell
{
	char c; //!< ASCII character.
	RGB f; //!< Foreground (character) color.
	RGB b; //!< Background color.

	/*!
		@brief Construct a `Cell`.
		@param character ASCII character.
		@param foreground Foreground (character) color.
		@param background Background color.
	*/
	constexpr Cell(char character = ' ', RGB foreground = RGBColors::black, RGB background = RGBColors::black)
		: c(character), f{foreground}, b(background) {}

	/*!
		@brief Compare 2 `Cell`s.
		@param cell The `Cell` to compare with this `Cell`.
		@return `true`: the foreground colors, background colors, and characters are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const Cell& cell) const -> bool
	{
		return (c == cell.c) && (f == cell.f) && (b == cell.b);
	}
};