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
#include "../basic_structs/rgba.hpp"

namespace KTech::RGBAColors
{
	constexpr RGBA red = RGBA(255, 0, 0, 255);
	constexpr RGBA orange = RGBA(255, 128, 0, 255);
	constexpr RGBA yellow = RGBA(255, 255, 0, 255);
	constexpr RGBA lime = RGBA(128, 255, 0, 255);
	constexpr RGBA green = RGBA(0, 255, 0, 255);
	constexpr RGBA mint = RGBA(0, 255, 128, 255);
	constexpr RGBA cyan = RGBA(0, 255, 255, 255);
	constexpr RGBA blue = RGBA(0, 128, 255, 255);
	constexpr RGBA primaryBlue = RGBA(0, 0, 255, 255);
	constexpr RGBA purple = RGBA(128, 0, 255, 255);
	constexpr RGBA pink = RGBA(255, 0, 255, 255);
	constexpr RGBA hotPink = RGBA(255, 0, 128, 255);
	constexpr RGBA white = RGBA(255, 255, 255, 255);
	constexpr RGBA gray = RGBA(160, 160, 160, 255);
	constexpr RGBA black = RGBA(0, 0, 0, 255);
	constexpr RGBA transparent = RGBA(0, 0, 0, 0);
}