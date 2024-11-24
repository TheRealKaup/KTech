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

struct KTech::RGBA
{
	uint8_t r, g, b, a;

	constexpr RGBA(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0)
		: r(red), g(green), b(blue), a(alpha) {}

	constexpr RGBA(RGB rgb, uint8_t alpha)
		: r(rgb.r), g(rgb.g), b(rgb.b), a(alpha) {}

	constexpr auto operator==(const RGBA& rgba) const -> bool
	{
		return (r == rgba.r) && (g == rgba.g) && (b == rgba.b) && (a == rgba.a);
	}

	constexpr auto operator!=(const RGBA& rgba) const -> bool
	{
		return !(*this == rgba);
	}
};