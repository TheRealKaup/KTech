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
#include "../utility/rgbcolors.hpp"
#include "../utility/rgbacolors.hpp"
#include "../engine/engine.hpp"

#include <iostream>
#include <fstream>

void KTech::Texture::Simple(UPoint _size, CellA _value, Point _pos) {
	simple = true;
	pos_r = _pos;
	size = _size;
	value = _value;
}

void KTech::Texture::Rectangle(UPoint _size, CellA _value, Point _pos)
{
	simple = false;
	pos_r = _pos;
	t.clear();
	t.resize(_size.y, std::vector<CellA>(_size.x, _value));
}

KTech::UPoint KTech::Texture::File(const std::string& fileName, Point _pos)
{
	simple = false;
	pos_r = _pos;

	IO::Log("<KTech::Texture::File()> Opening file " + fileName + "." , RGB(128, 128, 255));
	// Open file
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		IO::Log("<KTech::Texture::File()> Error! Failed to open file " + fileName + ".", RGB(255, 0, 0));
		t.resize(2, std::vector<CellA>(2));
		t[0][0].b = RGBA(255, 0, 220, 255);
		t[1][1].b = RGBA(255, 0, 220, 255);
		t[0][1].b = RGBA(0, 0, 0, 255);
		t[1][0].b = RGBA(0, 0, 0, 255);
		return UPoint(0, 0);
	}

	IO::Log("<KTech::Texture::File()> Reading size of texture.", RGB(128, 128, 255));
	// Get the texture size (first 8 bytes of the file)
	UPoint maxTextureSize;
	file.read((char*)&maxTextureSize, 8);
	IO::Log("<KTech::Texture::File()> Size of texture in file: { " + std::to_string(maxTextureSize.x) + ", " + std::to_string(maxTextureSize.y) + " }.", RGB(128, 128, 255));
	if (maxTextureSize.y > 9999)
	{
		IO::Log("<KTech::Texture::File()> Warning! Y size of texture in file is too big. Scaling Y down to 9999.", RGB(255, 0, 0));
		maxTextureSize.y = 9999;
	}
	if (maxTextureSize.x > 9999)
	{
		IO::Log("<KTech::Texture::File()> Warning! X size of texture in file is too big. Scaling X down to 9999.", RGB(255, 0, 0));
		maxTextureSize.x = 9999;
	}

	IO::Log("<KTech::Texture::File()> Resizing texture to fit texture in file.", RGB(128, 128, 255));
	// Resize the texture
	t.clear();
	t.resize(maxTextureSize.y, std::vector<CellA>(maxTextureSize.x));

	IO::Log("<KTech::Texture::File()> Reading rest of file and writing to texture.", RGB(128, 128, 255));
	// Read and write to the texture	
	for (size_t y = 0; y < t.size(); y++)
	{
		for (size_t x = 0; x < t[y].size(); x++)
		{
			if (file.read((char*)&t[y][x], 9).eof())
			{
				y = t.size();
				break;
			}
		}
	}

	// Print old texture file
	IO::Log("<KTech::Texture::File()> Printing texture:", RGB(128, 128, 255));
	for (std::vector<CellA>& row : t)
	{
		for (CellA& cell : row)
		{
			std::cout << "\033[38;2;" << std::to_string(cell.f.r * cell.f.a / 255) << ';' << std::to_string(cell.f.g * cell.f.a / 255) << ';'<< std::to_string(cell.f.b * cell.f.a / 255) << 'm'
				<< "\033[48;2;" << std::to_string(cell.b.r * cell.b.a / 255) << ';' << std::to_string(cell.b.g * cell.b.a / 255) << ';'<< std::to_string(cell.b.b * cell.b.a / 255) << 'm' << cell.c;
		}
		std::cout << "\033[m" << std::endl;
	}
	std::cout << std::flush;

	IO::Log("<KTech::Texture::File()> Returning.", RGB(128, 128, 255));
    return maxTextureSize;
}

void KTech::Texture::Write(const std::vector<std::string>& stringVector, RGBA frgba, RGBA brgba, Point _pos) {
	simple = false;
	pos_r = _pos;
	t.resize(stringVector.size());
	for (size_t y = 0; y < stringVector.size(); y++)
	{
		t[y].resize(stringVector[y].length());
		for (size_t x = 0; x < stringVector[y].length(); x++)
			t[y][x] = CellA(stringVector[y][x], (stringVector[y][x] == ' ' ? RGBA(RGBAColors::transparent) : frgba) , brgba);
	}	
}

void KTech::Texture::Resize(UPoint newSize, CellA _value)
{
	if (simple)
	{
		size = newSize;
		value = _value;
	}
	else
	{
		t.resize(newSize.y);
		for (std::vector<CellA>& row : t)
			row.resize(newSize.x, _value);
	}
}

void KTech::Texture::SetCell(CellA _value)
{
	if (simple)
		value = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x] = _value;
}

void KTech::Texture::SetForeground(RGBA _value)
{
	if (simple)
		value.f = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x].f = _value;
}

void KTech::Texture::SetBackground(RGBA _value)
{
	if (simple)
		value.b = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x].b = _value;
}

void KTech::Texture::SetCharacter(char _value)
{
	if (simple)
		value.c = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x].c = _value;
}

void KTech::Texture::SetForegroundAlpha(uint8_t _value)
{
	if (simple)
		value.f.a = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x].f.a = _value;
}

void KTech::Texture::SetBackgroundAlpha(uint8_t _value)
{
	if (simple)
		value.b.a = _value;
	else
		for (size_t y = 0; y < t.size(); y++)
			for (size_t x = 0; x < t[y].size(); x++)
				t[y][x].b.a = _value;
}

void KTech::Texture::SetAlpha(uint8_t _value)
{
	if (simple)
	{
		value.f.a = _value;
		value.b.a = _value;
	}
	else
	{
		for (size_t y = 0; y < t.size(); y++)
		{
			for (size_t x = 0; x < t[y].size(); x++)
			{
				t[y][x].f.a = _value;
				t[y][x].b.a = _value;
			}
		}
	}
}

KTech::UPoint KTech::Texture::GetSize() const
{
	// If the texture is simple, then `size` already represents the texture's size.
	if (simple)
		return size;
	// Otherwise, go over the entire texture to get the maximum X size, and use that to tell the size.
	UPoint size(0, t.size());
	for (size_t y = 0; y < t.size(); y++)
		if (t[y].size() > size.x)
			size.x = t[y].size();
	return size;
}

void KTech::Texture::ExportToFile(const std::string& fileName) const
{
	// Create/open file
	std::ofstream file(fileName);
	// Get the size of the texture
	UPoint maxTextureSize(0, t.size());
	for (size_t y = 0; y < t.size(); y++)
		if (t[y].size() > maxTextureSize.x)
			maxTextureSize.x = t[y].size();
	// Write the size of the texture at the start of the file
	file.write((char*)&maxTextureSize, 8);
	// Write the texture itself
	for (size_t y = 0; y < maxTextureSize.y; y++)
	{
		for (size_t x = 0; x < maxTextureSize.x; x++)
		{
			if (x < t[y].size())
				file.write((char*)&t[y][x], 9);
			else
				file.write("\0\0\0\0\0\0\0\0\0", 9);
		}
	}
}
