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
#include "../basic/rgb.hpp"

namespace KTech::RGBColors
{
	constexpr RGB red = RGB(255, 0, 0);
	constexpr RGB orange = RGB(255, 128, 0);
	constexpr RGB yellow = RGB(255, 255, 0);
	constexpr RGB lime = RGB(128, 255, 0);
	constexpr RGB green = RGB(0, 255, 0);
	constexpr RGB mint = RGB(0, 255, 128);
	constexpr RGB cyan = RGB(0, 255, 255);
	constexpr RGB blue = RGB(0, 128, 255);
	constexpr RGB primaryBlue = RGB(0, 0, 255);
	constexpr RGB purple = RGB(128, 0, 255);
	constexpr RGB magenta = RGB(255, 0, 220);
	constexpr RGB pink = RGB(255, 0, 255);
	constexpr RGB hotPink = RGB(255, 0, 128);
	constexpr RGB white = RGB(255, 255, 255);
	constexpr RGB gray = RGB(160, 160, 160);
	constexpr RGB black = RGB(0, 0, 0);
}