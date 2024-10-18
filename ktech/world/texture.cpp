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

KTech::Texture& KTech::Texture::Simple(UPoint p_size, CellA p_value)
{
	m_simple = true;
	m_size = p_size;
	m_value = p_value;
	return *this;
}

KTech::Texture& KTech::Texture::Simple(UPoint p_size, CellA p_value, Point p_pos)
{
	m_rPos = p_pos;
	return Simple(p_size, p_value);
}

KTech::Texture& KTech::Texture::Rectangle(UPoint p_size, CellA p_value)
{
	m_simple = false;
	m_size = p_size;
	m_t.resize(p_size.x * p_size.y);
	for (CellA& i : m_t)
		i = p_value;
	return *this;
}

KTech::Texture& KTech::Texture::Rectangle(UPoint p_size, CellA p_value, Point p_pos)
{
	m_rPos = p_pos;
	return Rectangle(p_size, p_value);
}

KTech::Texture& KTech::Texture::File(const std::filesystem::path& p_fileName)
{
	m_simple = false;
	// Open file
	std::ifstream file(p_fileName);
	if (!file.is_open()) 
		return Null(); // Failed to open file
	// Get size
	UPoint newSize;
	file.read((char*)&newSize, sizeof(UPoint));
	if (newSize.x * newSize.y * sizeof(CellA) != std::filesystem::file_size(p_fileName) - sizeof(UPoint))
		return Null(); // Size doesn't match real size
	// Apply size
	Resize(newSize);
	// Read from file
	file.read((char*)m_t.data(), m_t.size() * sizeof(CellA));
	return *this;
}

KTech::Texture& KTech::Texture::File(const std::filesystem::path& p_fileName, Point p_pos)
{
	m_rPos = p_pos;
	return File(p_fileName);
}

KTech::Texture& KTech::Texture::Write(const std::vector<std::string>& p_stringVector, RGBA p_frgba, RGBA p_brgba)
{
	m_simple = false;
	// Get size
	size_t maxX = 0;
	for (const std::string& row : p_stringVector)
		if (row.size() > maxX)
			maxX = row.size();
	// Apply size
	Resize(UPoint(maxX, p_stringVector.size()));
	// Read from strings
	for (size_t y = 0; y < m_size.y; y++)
		for (size_t x = 0; x < m_size.x; x++)
			operator()(x, y) = (x < p_stringVector[y].size()) ? CellA(p_stringVector[y][x], p_stringVector[y][x] != ' ' ? p_frgba : RGBA(0, 0, 0, 0), p_stringVector[y][x] != ' ' ? p_brgba : RGBA(0, 0, 0, 0)) : CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0));
	return *this;
}

KTech::Texture& KTech::Texture::Write(const std::vector<std::string>& p_stringVector, RGBA p_frgba, RGBA p_brgba, Point p_pos)
{
	m_rPos = p_pos;
	return Write(p_stringVector, p_frgba, p_brgba);
}

KTech::Texture& KTech::Texture::Null()
{
	Resize(UPoint(2, 2));
	m_t[0] = CellA(' ', RGBAColors::transparent, RGBA(255, 0, 220, 255));
	m_t[1] = CellA(' ', RGBAColors::transparent, RGBA(255, 0, 220, 255));
	m_t[2] = CellA(' ', RGBAColors::transparent, RGBA(0, 0, 0, 255));
	m_t[3] = CellA(' ', RGBAColors::transparent, RGBA(0, 0, 0, 255));
	return *this;
}

KTech::Texture& KTech::Texture::Null(Point p_pos)
{
	m_rPos = p_pos;
	return Null();
}

KTech::CellA& KTech::Texture::operator()(size_t x, size_t y)
{
	return m_t[m_size.x * y + x];
}

const KTech::CellA& KTech::Texture::operator()(size_t x, size_t y) const
{
	return m_t[m_size.x * y + x];
}

KTech::Texture& KTech::Texture::Resize(UPoint p_newSize, CellA p_value)
{
	if (m_simple)
	{
		m_size = p_newSize;
		m_value = p_value;
	}
	else
	{
		// New vector
		std::vector<CellA> newT(p_newSize.x * p_newSize.y);
		// Copy
		for (size_t x = 0; x < p_newSize.x; x++)
			for (size_t y = 0; y < p_newSize.y; y++)
				newT[p_newSize.x * y + x] = (x < m_size.x && y < m_size.y) ? m_t[m_size.x * y + x] : p_value;
		// Update size
		m_size = p_newSize;
		// Set to new vector
		m_t = newT;
	}
	return *this;
}

KTech::Texture& KTech::Texture::SetCell(CellA p_value)
{
	if (m_simple)
		m_value = p_value;
	else
		for (size_t i = 0; i < m_t.size(); i++)
			m_t[i] = p_value;
	return *this;
}

KTech::Texture& KTech::Texture::SetForeground(RGBA p_value)
{
	if (m_simple)
		m_value.f = p_value;
	else
		for (size_t i = 0; i < m_t.size(); i++)
			m_t[i].f = p_value;
	return *this;
}

KTech::Texture& KTech::Texture::SetBackground(RGBA p_value)
{
	if (m_simple)
		m_value.b = p_value;
	else
		for (size_t i = 0; i < m_t.size(); i++)
			m_t[i].b = p_value;
	return *this;
}

KTech::Texture& KTech::Texture::SetCharacter(char p_value)
{
	if (m_simple)
		m_value.c = p_value;
	else
		for (size_t i = 0; i < m_t.size(); i++)
			m_t[i].c = p_value;
	return *this;
}

KTech::Texture& KTech::Texture::SetForegroundAlpha(uint8_t p_value)
{
	if (m_simple)
		m_value.f.a = p_value;
	else
		for (size_t i = 0; i < m_t.size(); i++)
			m_t[i].f.a = p_value;
	return *this;
}

KTech::Texture& KTech::Texture::SetBackgroundAlpha(uint8_t p_value)
{
	if (m_simple)
		m_value.b.a = p_value;
	else
		for (size_t i = 0; i < m_t.size(); i++)
			m_t[i].b.a = p_value;
	return *this;
}

KTech::Texture& KTech::Texture::SetAlpha(uint8_t p_value)
{
	if (m_simple)
	{
		m_value.f.a = p_value;
		m_value.b.a = p_value;
	}
	else
	{
		for (size_t i = 0; i < m_t.size(); i++)
		{
			m_t[i].f.a = p_value;
			m_t[i].b.a = p_value;
		}
	}
	return *this;
}

KTech::Texture& KTech::Texture::ReplaceCharacter(char oldValue, char newValue)
{
	if (m_simple)
	{
		if (m_value.c == oldValue)
			m_value.c = newValue;
	}
	else
	{
		for (size_t i = 0; i < m_t.size(); i++)
			if (m_t[i].c == oldValue)
				m_t[i].c = newValue;
	}
	return *this;
}

void KTech::Texture::ExportToFile(const std::string& p_fileName) const
{
	// Create/open file
	std::ofstream file(p_fileName);
	// Write size
	file.write((char*)&m_size, 8);
	// Write texture
	file.write((char*)m_t.data(), m_t.size() * 9);
}

void KTech::Texture::Print()
{
	for (size_t i = 0; i < m_t.size();)
	{
		for (size_t x = 0; x < m_size.x; x++, i++)
		{
			std::cout << "\033[38;2;" << std::to_string(m_t[i].f.r * m_t[i].f.a / 255) << ';' << std::to_string(m_t[i].f.g * m_t[i].f.a / 255) << ';'<< std::to_string(m_t[i].f.b * m_t[i].f.a / 255) << 'm'
				<< "\033[48;2;" << std::to_string(m_t[i].b.r * m_t[i].b.a / 255) << ';' << std::to_string(m_t[i].b.g * m_t[i].b.a / 255) << ';'<< std::to_string(m_t[i].b.b * m_t[i].b.a / 255) << 'm' << (' ' <= m_t[i].c && m_t[i].c <= '~' ? m_t[i].c : ' ');
		}
		std::cout << "\033[m" << std::endl;
	}
}