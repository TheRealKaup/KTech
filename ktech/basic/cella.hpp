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

struct KTech::CellA
{
	RGBA f;
	RGBA b;
	char c;

	constexpr CellA(char character = ' ', RGBA foreground = RGBAColors::transparent, RGBA background = RGBAColors::transparent)
		: c(character), f(foreground), b(background) {}

	constexpr CellA(Cell cell, uint8_t foregroundAlpha, uint8_t backgroundAlpha)
		: c(cell.c), f(cell.f, foregroundAlpha), b(cell.b, backgroundAlpha) {}

	constexpr auto operator==(const CellA& cellA) const -> bool
	{
		return (c == cellA.c) && (f == cellA.f) && (b == cellA.b);
	}

	constexpr auto operator!=(const CellA& cellA) const -> bool
	{
		return !(*this == cellA);
	}
};