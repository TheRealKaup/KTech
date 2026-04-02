/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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

/*!
	@file
	@brief Define KTech::Texture members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "texture.hpp"

#include "../utility/rgbacolors.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
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

auto KTech::Texture::Write(const std::vector<std::string>& p_stringVector, RGBA p_foreground, RGBA p_background)
	-> Texture&
{
	m_simple = false;
	// Get size
	size_t maxX = 0;
	for (const std::string& row : p_stringVector)
	{
		maxX = std::max(row.size(), maxX);
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

auto KTech::Texture::Write(
	const std::vector<std::string>& p_stringVector, RGBA p_foreground, RGBA p_background, Point p_relativePosition
) -> Texture&
{
	m_rPos = p_relativePosition;
	return Write(p_stringVector, p_foreground, p_background);
}

auto KTech::Texture::Null() -> Texture&
{
	Resize(UPoint(2, 2));
	m_t[0] = CellA(' ', RGBAColors::transparent, RGBAColors::black);
	m_t[1] = CellA(' ', RGBAColors::transparent, RGBAColors::magenta);
	m_t[2] = CellA(' ', RGBAColors::transparent, RGBAColors::magenta);
	m_t[3] = CellA(' ', RGBAColors::transparent, RGBAColors::black);
	return *this;
}

auto KTech::Texture::Null(Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Null();
}

auto KTech::Texture::operator()(size_t p_x, size_t p_y) -> CellA&
{
	return m_t[(m_size.x * p_y) + p_x];
}

auto KTech::Texture::operator()(size_t x, size_t y) const -> const CellA&
{
	return m_t[(m_size.x * y) + x];
}

auto KTech::Texture::Resize(UPoint p_size, CellA p_newValue) -> Texture&
{
	if (m_simple)
	{
		m_size = p_size;
	}
	else
	{
		// CREATE new vector
		std::vector<CellA> newT(p_size.x * p_size.y);
		// COPY
		for (size_t y = 0; y < p_size.y; y++)
		{
			for (size_t x = 0; x < p_size.x; x++)
			{
				newT[(p_size.x * y) + x] = (x < m_size.x && y < m_size.y) ? m_t[(m_size.x * y) + x] : p_newValue;
			}
		}
		// UPDATE size
		m_size = p_size;
		// MOVE new vector
		m_t = std::move(newT);
	}
	return *this;
}

auto KTech::Texture::Transform(const std::function<void(CellA&)>& p_operation, UPoint p_from, UPoint p_to) -> Texture&
{
	if (m_simple)
	{
		// CALL operation once with `m_value`
		p_operation(m_value);
		// RETURN self-reference
		return *this;
	}

	if (p_to == UPoint(0, 0))
	{
		// DEFAULT bottom-right corner
		p_to = m_size;
	}
	else
	{
		// LIMIT bottom-right corner
		p_to.x = std::min(p_to.x, m_size.x);
		p_to.y = std::min(p_to.y, m_size.y);
	}
	if (p_from.x >= p_to.x || p_from.y >= p_to.y)
	{
		// RETURN self-reference because top-left corner is out of range.
		return *this;
	}

	for (UPoint pos{p_from}; pos.y < p_to.y; pos.y++)
	{
		for (pos.x = p_from.x; pos.x < p_to.x; pos.x++)
		{
			// CALL operation with each cell in the range
			p_operation(m_t[(pos.y * m_size.x) + pos.x]);
		}
	}
	// RETURN self-reference
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

void KTech::Texture::Print() const
{
	if (m_simple)
	{
		std::string cellString = "\033[38;2;"
							   + std::to_string(m_value.f.r * m_value.f.a / std::numeric_limits<uint8_t>::max())
							   + ';'
							   + std::to_string(m_value.f.g * m_value.f.a / std::numeric_limits<uint8_t>::max())
							   + ';'
							   + std::to_string(m_value.f.b * m_value.f.a / std::numeric_limits<uint8_t>::max())
							   + "m\033[48;2;"
							   + std::to_string(m_value.b.r * m_value.b.a / std::numeric_limits<uint8_t>::max())
							   + ';'
							   + std::to_string(m_value.b.g * m_value.b.a / std::numeric_limits<uint8_t>::max())
							   + ';'
							   + std::to_string(m_value.b.b * m_value.b.a / std::numeric_limits<uint8_t>::max())
							   + 'm'
							   + (' ' <= m_value.c && m_value.c <= '~' ? m_value.c : ' ');

		for (size_t x = 0; x < m_size.x; x++)
		{
			for (size_t y = 0; y < m_size.y; y++)
			{
				std::cout << cellString;
			}
			std::cout << "\033[m\n";
		}
	}
	else
	{
		for (size_t i = 0; i < m_t.size();)
		{
			for (size_t x = 0; x < m_size.x; x++, i++)
			{
				std::cout
					<< "\033[38;2;"
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
	std::cout << std::flush;
}
