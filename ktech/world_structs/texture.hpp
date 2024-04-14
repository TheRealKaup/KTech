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
#include "../basic_structs/point.hpp"
#include "../basic_structs/upoint.hpp"
#include "../basic_structs/rgba.hpp"
#include "../basic_structs/cella.hpp"

#include <string>
#include <vector>

struct KTech::Texture
{
	bool active = true;
	Point pos_r = {0, 0};
	bool simple = true;

	UPoint size = { 0, 0 };
	CellA value = CellA('#', RGBA());

	std::vector<std::vector<CellA>> t = {}; 
	
	void Simple(UPoint size, CellA value, Point relative_position = Point(0, 0));
	void Rectangle(UPoint size, CellA value, Point relative_position = Point(0, 0));// Load from a file.
	UPoint File(const std::string& fileName, Point relative_position = Point(0, 0));
	void Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point relative_position = Point(0, 0));

	void Resize(UPoint newSize, CellA newValue = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)));
	void SetCell(CellA value);
	void SetForeground(RGBA value);
	void SetBackground(RGBA value);
	void SetCharacter(char value);
	void SetForegroundAlpha(uint8_t value);
	void SetBackgroundAlpha(uint8_t value);
	void SetAlpha(uint8_t value);
	
	UPoint GetSize() const;
	void ExportToFile(const std::string& fileName) const;
};