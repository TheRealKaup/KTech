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
#include "cell.hpp"
#include "rgba.hpp"
#include "../utility/rgbacolors.hpp"

//! Like `Cell`, but with `RGBA` foreground and background colors, instead of `RGB`.
struct KTech::CellA
{
	RGBA f; //!< Foreground (character) color.
	RGBA b; //!< Background color.
	char c; //!< ASCII character.

	/*!
		@brief Construct a `CellA`.
		@param [in] character ASCII character.
		@param [in] foreground Foreground (character) color.
		@param [in] background Background color.
	*/
	constexpr CellA(char character = ' ', RGBA foreground = RGBAColors::transparent, RGBA background = RGBAColors::transparent)
		: c(character), f(foreground), b(background) {}

	/*!
		@brief Construct a `CellA` from a `Cell`.
		@param [in] cell Base `Cell`.
		@param [in] foregroundAlpha Alpha to add to the foreground (character) color.
		@param [in] backgroundAlpha Alpha to add to the background color.
	*/
	constexpr CellA(Cell cell, uint8_t foregroundAlpha, uint8_t backgroundAlpha)
		: c(cell.c), f(cell.f, foregroundAlpha), b(cell.b, backgroundAlpha) {}

	/*!
		@brief Compare 2 `CellA`s.
		@param [in] cellA The `CellA` to compare with this `CellA`.
		@return `true`: the foreground colors, background colors, and characters are equal. `false`: they are unequal.
	*/
	constexpr auto operator==(const CellA& cellA) const -> bool
	{
		return (c == cellA.c) && (f == cellA.f) && (b == cellA.b);
	}
};