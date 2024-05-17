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
#include "../basic/rgba.hpp"
#include "../basic/cella.hpp"

#include <string>
#include <vector>

struct KTech::Texture
{
	bool m_active = true;

	Point m_rPos;
	bool m_simple;
	UPoint m_size;
	CellA m_value;
	std::vector<std::vector<CellA>> m_t;
	
	void Simple(UPoint size, CellA value);
	void Simple(UPoint size, CellA value, Point relateivePosition);
	void Rectangle(UPoint size, CellA value);
	void Rectangle(UPoint size, CellA value, Point relateivePosition);
	UPoint File(const std::string& fileName);
	UPoint File(const std::string& fileName, Point relateivePosition);
	UPoint Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background);
	UPoint Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background, Point relateivePosition);

	std::vector<CellA>& operator[](size_t y);
	const std::vector<CellA>& operator[](size_t y) const;

	UPoint GetSize() const;

	// `newValue` - only fills the new cells.
	void Resize(UPoint size, CellA newValue = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)));
	void SetCell(CellA value);
	void SetForeground(RGBA value);
	void SetBackground(RGBA value);
	void SetCharacter(char value);
	void SetForegroundAlpha(uint8_t value);
	void SetBackgroundAlpha(uint8_t value);
	void SetAlpha(uint8_t value);
	void ReplaceCharacter(char oldValue, char newValue);
	
	void ExportToFile(const std::string& fileName) const;
	// For debugging
	void Print();
};