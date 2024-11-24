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
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"
#include "../basic/rgb.hpp"
#include "../basic/rgba.hpp"

#include <limits>

constexpr uint8_t uint8Max = std::numeric_limits<uint8_t>::max();

// Delimits the rectangle formed by `start` and `end` within `res`, assuming `start` is equal to `end` or that it is closer to (0, 0) than `end` (i.e. `start` is smaller).
// Returns false if the rectangle formed by `start` and `end` doesn't overlap at all with `res` (i.e. the rectangle formed by (0, 0) and `end.`)
// In the case that it returns false, `start` and `end` might be partially modified and deemed obsolete.
static inline auto Delimit(KTech::Point& p_start, KTech::Point& p_end, KTech::UPoint& p_res) -> bool
{
	if (p_start.x < 0)
	{
		p_start.x = 0;
	}
	else if (p_start.x > static_cast<int>(p_res.x))
	{
		return false;
	}
	if (p_start.y < 0)
	{
		p_start.y = 0;
	}
	else if (p_start.y > static_cast<int>(p_res.y))
	{
		return false;
	}
	if (p_end.x > static_cast<int>(p_res.x))
	{
		p_end.x = p_res.x;
	}
	else if (p_end.x < 0)
	{
		return false;
	}
	if (p_end.y > static_cast<int>(p_res.y))
	{
		p_end.y = p_res.y;
	}
	else if (p_end.y < 0)
	{
		return false;
	}
	return true;
}

// Returns false if space (' '). Returns true if printable or fixed.
static inline auto DetermineCharacter(char& p_char) -> bool
{
	if (p_char == ' ')
	{
		return false;
	}
	if (p_char < ' ' || '~' < p_char)
	{
		p_char = ' ';
	}
	return true;
}

// Populates `dst` with `src`, after calculating RGB values based on `src` alpha channel.
// Returns false if alpha channel is 0 (`dst` isn't changed). Returns true otherwise.
// Overriding `BakeRGBA` would impair readability.
static inline auto BakeRGBA(KTech::RGBA& p_dst, const KTech::RGBA& p_src) -> bool
{
	if (p_src.a == 0) // Won't add anything; return false so no needless drawing will be done
	{
		return false;
	}
	p_dst.a = p_src.a;
	p_dst.r = p_src.r * p_dst.a / uint8Max; // Bake alpha into RGB
	p_dst.g = p_src.g * p_dst.a / uint8Max;
	p_dst.b = p_src.b * p_dst.a / uint8Max;
	return true; // Will add something; return true so drawing will be done
}

// Populates `dst` with `src`, after calculating RGB values based on `src` alpha channel and additional `m_alpha`.
// Returns false if alpha channel is 0 (still sets `dst.a`). Returns true otherwise.
static inline auto BakeRGBAWith(KTech::RGBA& p_dst, const KTech::RGBA& p_src, uint8_t p_alpha) -> bool
{
	p_dst.a = p_src.a * p_alpha / uint8Max; // Take `Layer::m_alpha` into consideration
	if (p_dst.a == 0) // Won't add anything; return false so no needless drawing will be done
	{
		return false;
	}
	p_dst.r = p_src.r * p_dst.a / uint8Max; // Bake alpha into RGB
	p_dst.g = p_src.g * p_dst.a / uint8Max;
	p_dst.b = p_src.b * p_dst.a / uint8Max;
	return true; // Will add something; return true so drawing will be done
}

// Draws `src` on `dst`, with respect to alpha channels.
// Assumes `src.a` was baked into the RGB values of `src` already ("baked").
static inline void DrawBakedToRGB(KTech::RGB& p_dst, const KTech::RGBA& p_src)
{
	p_dst.r = p_src.r + p_dst.r * (uint8Max - p_src.a) / uint8Max;
	p_dst.g = p_src.g + p_dst.g * (uint8Max - p_src.a) / uint8Max;
	p_dst.b = p_src.b + p_dst.b * (uint8Max - p_src.a) / uint8Max;
}

// Draws `src` on `dst`, with respect to alpha channels.
// Assumes `src.a` was baked into the RGB values of `src` already ("baked").
static inline void DrawBakedToRGBA(KTech::RGBA& p_dst, const KTech::RGBA& p_src)
{
	p_dst.r = p_src.r + p_dst.r * (uint8Max - p_src.a) / uint8Max;
	p_dst.g = p_src.g + p_dst.g * (uint8Max - p_src.a) / uint8Max;
	p_dst.b = p_src.b + p_dst.b * (uint8Max - p_src.a) / uint8Max;
	p_dst.a += (uint8Max - p_dst.a) * p_src.a / uint8Max;
}

static inline auto GetThirdDigit(uint8_t uint8) -> char
{
	return uint8 / 100 + '0';
}

static inline auto GetSecondDigit(uint8_t uint8) -> char
{
	return (uint8 % 100) / 10 + '0';
}

static inline auto GetFirstDigit(uint8_t uint8) -> char
{
	return uint8 % 10 + '0';
}