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

#include <string>
#include <vector>

struct KTech::Collider
{
	bool m_active = true;

	Point m_rPos;
	bool m_simple;
	UPoint m_size;
	std::vector<std::vector<bool>> m_c;
	uint8_t m_type;

	void Simple(UPoint size, uint8_t type, Point relativePosition = Point(0, 0));
	bool File(const std::string& fileName, uint8_t type, Point relativePosition = Point(0, 0));
	void Write(const std::vector<std::string>& stringVector, uint8_t type, Point relativePosition = Point(0, 0));
	void ByTextureCharacter(const Texture& texture, uint8_t alphaThreshold, uint8_t type);
	void ByTextureBackground(const Texture& texture, uint8_t alphaThreshold, uint8_t type);

	UPoint GetSize() const;
};