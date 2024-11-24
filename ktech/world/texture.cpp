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

#include "texture.hpp"

#include "../utility/rgbacolors.hpp"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <limits>

auto KTech::Texture::Simple(UPoint p_size, CellA p_value) -> Texture&
{
	m_simple = true;
	m_size = p_size;
	m_value = p_value;
	return *this;
}

auto KTech::Texture::Simple(UPoint p_size, CellA p_value, Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Simple(p_size, p_value);
}

auto KTech::Texture::Rectangle(UPoint p_size, CellA p_value) -> Texture&
{
	m_simple = false;
	m_size = p_size;
	m_t.resize(p_size.x * p_size.y);
	for (CellA& cell : m_t)
	{
		cell = p_value;
	}
	return *this;
}

auto KTech::Texture::Rectangle(UPoint p_size, CellA p_value, Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Rectangle(p_size, p_value);
}

auto KTech::Texture::File(const std::filesystem::path& p_filePath) -> Texture&
{
	m_simple = false;
	// Open file
	std::ifstream file(p_filePath);
	if (!file.is_open())
	{
		return Null(); // Failed to open file
	}
	// Get size
	UPoint newSize;
	file.read((char*)&newSize, sizeof(UPoint));
	if (newSize.x * newSize.y * sizeof(CellA) != std::filesystem::file_size(p_filePath) - sizeof(UPoint))
	{
		return Null(); // Size doesn't match real size
	}
	// Apply size
	Resize(newSize);
	// Read from file
	file.read((char*)m_t.data(), m_t.size() * sizeof(CellA));
	return *this;
}

auto KTech::Texture::File(const std::filesystem::path& p_filePath, Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return File(p_filePath);
}

auto KTech::Texture::Write(const std::vector<std::string>& p_stringVector, RGBA p_foreground, RGBA p_background) -> Texture&
{
	m_simple = false;
	// Get size
	size_t maxX = 0;
	for (const std::string& row : p_stringVector)
	{
		if (row.size() > maxX)
		{
			maxX = row.size();
		}
	}
	// Apply size
	Resize(UPoint(maxX, p_stringVector.size()));
	// Read from strings
	for (size_t y = 0; y < m_size.y; y++)
	{
		for (size_t x = 0; x < m_size.x; x++)
		{
			if (x < p_stringVector[y].size())
			{
				if (p_stringVector[y][x] == ' ')
				{
					operator()(x, y) = CellA(p_stringVector[y][x], RGBAColors::transparent, RGBAColors::transparent);
				}
				else
				{
					operator()(x, y) = CellA(p_stringVector[y][x], p_foreground, p_background);
				}
			}
			else
			{
				operator()(x, y) = CellA(' ', RGBAColors::transparent, RGBAColors::transparent);
			}
		}
	}
	return *this;
}

auto KTech::Texture::Write(const std::vector<std::string>& p_stringVector, RGBA p_foreground, RGBA p_background, Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Write(p_stringVector, p_foreground, p_background);
}

auto KTech::Texture::Null() -> Texture&
{
	Resize(UPoint(2, 2));
	m_t[0] = CellA(' ', RGBAColors::transparent, RGBAColors::magenta);
	m_t[1] = CellA(' ', RGBAColors::transparent, RGBAColors::magenta);
	m_t[2] = CellA(' ', RGBAColors::transparent, RGBAColors::black);
	m_t[3] = CellA(' ', RGBAColors::transparent, RGBAColors::black);
	return *this;
}

auto KTech::Texture::Null(Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Null();
}

auto KTech::Texture::operator()(size_t x, size_t y) -> CellA&
{
	return m_t[m_size.x * y + x];
}

auto KTech::Texture::operator()(size_t x, size_t y) const -> const CellA&
{
	return m_t[m_size.x * y + x];
}

auto KTech::Texture::Resize(UPoint p_size, CellA p_newValue) -> Texture&
{
	if (m_simple)
	{
		m_size = p_size;
		m_value = p_newValue;
	}
	else
	{
		// New vector
		std::vector<CellA> newT(p_size.x * p_size.y);
		// Copy
		for (size_t x = 0; x < p_size.x; x++)
		{
			for (size_t y = 0; y < p_size.y; y++)
			{
				newT[p_size.x * y + x] = (x < m_size.x && y < m_size.y) ? m_t[m_size.x * y + x] : p_newValue;
			}
		}
		// Update size
		m_size = p_size;
		// Set to new vector
		m_t = newT;
	}
	return *this;
}

auto KTech::Texture::SetCell(CellA p_value) -> Texture&
{
	if (m_simple)
	{
		m_value = p_value;
	}
	else
	{
		for (CellA& cell : m_t)
		{
			cell = p_value;
		}
	}
	return *this;
}

auto KTech::Texture::SetForeground(RGBA p_value) -> Texture&
{
	if (m_simple)
	{
		m_value.f = p_value;
	}
	else
	{
		for (CellA& cell : m_t)
		{
			cell.f = p_value;
		}
	}
	return *this;
}

auto KTech::Texture::SetBackground(RGBA p_value) -> Texture&
{
	if (m_simple)
	{
		m_value.b = p_value;
	}
	else
	{
		for (CellA& cell : m_t)
		{
			cell.b = p_value;
		}
	}
	return *this;
}

auto KTech::Texture::SetCharacter(char p_value) -> Texture&
{
	if (m_simple)
	{
		m_value.c = p_value;
	}
	else
	{
		for (CellA& cell : m_t)
		{
			cell.c = p_value;
		}
	}
	return *this;
}

auto KTech::Texture::SetForegroundAlpha(uint8_t p_value) -> Texture&
{
	if (m_simple)
	{
		m_value.f.a = p_value;
	}
	else
	{
		for (CellA& cell : m_t)
		{
			cell.f.a = p_value;
		}
	}
	return *this;
}

auto KTech::Texture::SetBackgroundAlpha(uint8_t p_value) -> Texture&
{
	if (m_simple)
	{
		m_value.b.a = p_value;
	}
	else
	{
		for (CellA& cell : m_t)
		{
			cell.b.a = p_value;
		}
	}
	return *this;
}

auto KTech::Texture::SetAlpha(uint8_t p_value) -> Texture&
{
	if (m_simple)
	{
		m_value.f.a = p_value;
		m_value.b.a = p_value;
	}
	else
	{
		for (CellA& cell : m_t)
		{
			cell.f.a = p_value;
			cell.b.a = p_value;
		}
	}
	return *this;
}

auto KTech::Texture::ReplaceCharacter(char oldValue, char newValue) -> Texture&
{
	if (m_simple)
	{
		if (m_value.c == oldValue)
		{
			m_value.c = newValue;
		}
	}
	else
	{
		for (CellA& cell : m_t)
		{
			if (cell.c == oldValue)
			{
				cell.c = newValue;
			}
		}
	}
	return *this;
}

void KTech::Texture::ExportToFile(const std::filesystem::path& p_filePath) const
{
	// Create/open file
	std::ofstream file(p_filePath);
	if (file.is_open())
	{
		constexpr size_t sizeNBytes = 8;
		constexpr size_t cellNBytes = 9;
		// Write size
		file.write((char*)&m_size, sizeNBytes);
		// Write texture
		file.write((char*)m_t.data(), m_t.size() * cellNBytes);
	}
}

void KTech::Texture::Print()
{
	for (size_t i = 0; i < m_t.size();)
	{
		for (size_t x = 0; x < m_size.x; x++, i++)
		{
			std::cout << "\033[38;2;"
				<< std::to_string(m_t[i].f.r * m_t[i].f.a / std::numeric_limits<uint8_t>::max())
				<< ';'
				<< std::to_string(m_t[i].f.g * m_t[i].f.a / std::numeric_limits<uint8_t>::max())
				<< ';'
				<< std::to_string(m_t[i].f.b * m_t[i].f.a / std::numeric_limits<uint8_t>::max())
				<< "m\033[48;2;"
				<< std::to_string(m_t[i].b.r * m_t[i].b.a / std::numeric_limits<uint8_t>::max())
				<< ';'
				<< std::to_string(m_t[i].b.g * m_t[i].b.a / std::numeric_limits<uint8_t>::max())
				<< ';'
				<< std::to_string(m_t[i].b.b * m_t[i].b.a / std::numeric_limits<uint8_t>::max())
				<< 'm'
				<< (' ' <= m_t[i].c && m_t[i].c <= '~' ? m_t[i].c : ' ');
		}
		std::cout << "\033[m\n";
	}
}