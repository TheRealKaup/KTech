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
#include "../engine/output.hpp"

#include <fstream>
#include <iostream>

void KTech::Texture::Simple(UPoint p_size, CellA p_value, Point p_pos) {
	m_simple = true;
	m_rPos = p_pos;
	m_size = p_size;
	m_value = p_value;
}

void KTech::Texture::Rectangle(UPoint p_size, CellA p_value, Point p_pos)
{
	m_simple = false;
	m_rPos = p_pos;
	m_t.clear();
	m_t.resize(p_size.y, std::vector<CellA>(p_size.x, p_value));
}

KTech::UPoint KTech::Texture::File(const std::string& p_fileName, Point p_pos)
{
	m_simple = false;
	m_rPos = p_pos;
	m_size = UPoint(0, 0);

	Output::Log("<KTech::Texture::File()> Opening file " + p_fileName + "." , RGB(128, 128, 255));
	// Open file
	std::ifstream file(p_fileName);
	if (!file.is_open())
	{
		Output::Log("<KTech::Texture::File()> Error! Failed to open file " + p_fileName + ".", RGB(255, 0, 0));
		m_t.resize(2, std::vector<CellA>(2));
		m_t[0][0].b = RGBA(255, 0, 220, 255);
		m_t[1][1].b = RGBA(255, 0, 220, 255);
		m_t[0][1].b = RGBA(0, 0, 0, 255);
		m_t[1][0].b = RGBA(0, 0, 0, 255);
		return UPoint(0, 0);
	}

	Output::Log("<KTech::Texture::File()> Reading size of texture.", RGB(128, 128, 255));
	// Get the texture size (first 8 bytes of the file)
	UPoint maxTextureSize;
	file.read((char*)&maxTextureSize, 8);
	Output::Log("<KTech::Texture::File()> Size of texture in file: { " + std::to_string(maxTextureSize.x) + ", " + std::to_string(maxTextureSize.y) + " }.", RGB(128, 128, 255));
	if (maxTextureSize.y > 9999)
	{
		Output::Log("<KTech::Texture::File()> Warning! Y size of texture in file is too big. Scaling Y down to 9999.", RGB(255, 0, 0));
		maxTextureSize.y = 9999;
	}
	if (maxTextureSize.x > 9999)
	{
		Output::Log("<KTech::Texture::File()> Warning! X size of texture in file is too big. Scaling X down to 9999.", RGB(255, 0, 0));
		maxTextureSize.x = 9999;
	}

	Output::Log("<KTech::Texture::File()> Resizing texture to fit texture in file.", RGB(128, 128, 255));
	// Resize the texture
	m_t.clear();
	m_t.resize(maxTextureSize.y, std::vector<CellA>(maxTextureSize.x));

	Output::Log("<KTech::Texture::File()> Reading rest of file and writing to texture.", RGB(128, 128, 255));
	// Read and write to the texture	
	for (size_t y = 0; y < m_t.size(); y++)
	{
		for (size_t x = 0; x < m_t[y].size(); x++)
		{
			// Backwards compatability
			char data[9];
			file.read(data, 9);
			m_t[y][x].f.r = data[0];
			m_t[y][x].f.g = data[1];
			m_t[y][x].f.b = data[2];
			m_t[y][x].f.a = data[3];
			m_t[y][x].b.r = data[4];
			m_t[y][x].b.g = data[5];
			m_t[y][x].b.b = data[6];
			m_t[y][x].b.a = data[7];
			m_t[y][x].c = data[8];
			if (file.eof())
			{
				y = m_t.size();
				break;
			}
		}
	}

	// Print old texture file
	Output::Log("<KTech::Texture::File()> Printing texture:", RGB(128, 128, 255));
	for (std::vector<CellA>& row : m_t)
	{
		for (CellA& cell : row)
		{
			std::cout << "\033[38;2;" << std::to_string(cell.f.r * cell.f.a / 255) << ';' << std::to_string(cell.f.g * cell.f.a / 255) << ';'<< std::to_string(cell.f.b * cell.f.a / 255) << 'm'
				<< "\033[48;2;" << std::to_string(cell.b.r * cell.b.a / 255) << ';' << std::to_string(cell.b.g * cell.b.a / 255) << ';'<< std::to_string(cell.b.b * cell.b.a / 255) << 'm' << cell.c;
		}
		std::cout << "\033[m" << std::endl;
	}
	std::cout << std::flush;

	Output::Log("<KTech::Texture::File()> Returning.", RGB(128, 128, 255));
    return maxTextureSize;
}

void KTech::Texture::Write(const std::vector<std::string>& p_stringVector, RGBA p_frgba, RGBA p_brgba, Point p_pos) {
	m_simple = false;
	m_rPos = p_pos;
	m_t.resize(p_stringVector.size());
	m_size = UPoint(0, 0);

	for (size_t y = 0; y < p_stringVector.size(); y++)
	{
		m_t[y].resize(p_stringVector[y].length());
		for (size_t x = 0; x < p_stringVector[y].length(); x++)
			m_t[y][x] = CellA(p_stringVector[y][x], (p_stringVector[y][x] == ' ' ? RGBA(RGBAColors::transparent) : p_frgba) , p_brgba);
	}	
}

KTech::UPoint KTech::Texture::GetSize() const
{
	// If the texture is simple, then `size` already represents the texture's size.
	if (m_simple)
		return m_size;
	// Otherwise, go over the entire texture to get the maximum X size, and use that to tell the size.
	UPoint size(0, m_t.size());
	for (size_t y = 0; y < m_t.size(); y++)
		if (m_t[y].size() > size.x)
			size.x = m_t[y].size();
	return size;
}

void KTech::Texture::Resize(UPoint p_newSize, CellA p_value)
{
	if (m_simple)
	{
		m_size = p_newSize;
		m_value = p_value;
	}
	else
	{
		m_t.resize(p_newSize.y);
		for (std::vector<CellA>& row : m_t)
			row.resize(p_newSize.x, p_value);
	}
}

void KTech::Texture::SetCell(CellA p_value)
{
	if (m_simple)
		m_value = p_value;
	else
		for (size_t y = 0; y < m_t.size(); y++)
			for (size_t x = 0; x < m_t[y].size(); x++)
				m_t[y][x] = p_value;
}

void KTech::Texture::SetForeground(RGBA p_value)
{
	if (m_simple)
		m_value.f = p_value;
	else
		for (size_t y = 0; y < m_t.size(); y++)
			for (size_t x = 0; x < m_t[y].size(); x++)
				m_t[y][x].f = p_value;
}

void KTech::Texture::SetBackground(RGBA p_value)
{
	if (m_simple)
		m_value.b = p_value;
	else
		for (size_t y = 0; y < m_t.size(); y++)
			for (size_t x = 0; x < m_t[y].size(); x++)
				m_t[y][x].b = p_value;
}

void KTech::Texture::SetCharacter(char p_value)
{
	if (m_simple)
		m_value.c = p_value;
	else
		for (size_t y = 0; y < m_t.size(); y++)
			for (size_t x = 0; x < m_t[y].size(); x++)
				m_t[y][x].c = p_value;
}

void KTech::Texture::SetForegroundAlpha(uint8_t p_value)
{
	if (m_simple)
		m_value.f.a = p_value;
	else
		for (size_t y = 0; y < m_t.size(); y++)
			for (size_t x = 0; x < m_t[y].size(); x++)
				m_t[y][x].f.a = p_value;
}

void KTech::Texture::SetBackgroundAlpha(uint8_t p_value)
{
	if (m_simple)
		m_value.b.a = p_value;
	else
		for (size_t y = 0; y < m_t.size(); y++)
			for (size_t x = 0; x < m_t[y].size(); x++)
				m_t[y][x].b.a = p_value;
}

void KTech::Texture::SetAlpha(uint8_t p_value)
{
	if (m_simple)
	{
		m_value.f.a = p_value;
		m_value.b.a = p_value;
	}
	else
	{
		for (size_t y = 0; y < m_t.size(); y++)
		{
			for (size_t x = 0; x < m_t[y].size(); x++)
			{
				m_t[y][x].f.a = p_value;
				m_t[y][x].b.a = p_value;
			}
		}
	}
}

void KTech::Texture::ExportToFile(const std::string& p_fileName) const
{
	// Create/open file
	std::ofstream file(p_fileName);
	// Get the size of the texture
	UPoint maxTextureSize(0, m_t.size());
	for (size_t y = 0; y < m_t.size(); y++)
		if (m_t[y].size() > maxTextureSize.x)
			maxTextureSize.x = m_t[y].size();
	// Write the size of the texture at the start of the file
	file.write((char*)&maxTextureSize, 8);
	// Write the texture itself
	for (size_t y = 0; y < maxTextureSize.y; y++)
	{
		for (size_t x = 0; x < maxTextureSize.x; x++)
		{
			if (x < m_t[y].size())
				file.write((char*)&m_t[y][x], 9);
			else
				file.write("\0\0\0\0\0\0\0\0\0", 9);
		}
	}
}
