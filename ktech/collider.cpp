/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 E. Kaufman (AKA Kaup)

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

#include "ktech.hpp"

void KTech::Collider::Simple(UPoint _size, uint8_t _type, Point _pos)
{
    simple = true;
    type = _type;
    pos_r = _pos;
    size = _size;
}

bool KTech::Collider::File(const std::string& fileName, uint8_t _type, Point _pos)
{
    simple = false;
    type = _type;
    pos_r = _pos;

	std::ifstream file(fileName);
	if (!file.is_open())
		return false;
	std::string line;

	for (size_t y = 0; std::getline(file, line); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		c.resize(c.size() + 1);
		c[y].resize(line.length());
		for (size_t x = 0; x < line.length(); x++)
			c[y][x] = line[x];
	}
	
	return true;
}

void KTech::Collider::Write(const std::vector<std::string>& stringVector, uint8_t _type, Point _pos)
{
    simple = false;
    type = _type;
    pos_r = _pos;

	c.resize(stringVector.size());
	for (size_t y = 0; y < stringVector.size(); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		c[y].resize(stringVector[y].length());
		for (size_t x = 0; x < c[y].size(); x++)
			c[y][x] = stringVector[y][x] == ' ' ? false : true;
	}
}

void KTech::Collider::ByTextureCharacter(const Texture& texture, uint8_t alphaThreshold, uint8_t _type)
{
    simple = false;
    type = _type;
    pos_r = texture.pos_r;

	c.resize(texture.t.size());
	for (size_t y = 0; y < texture.t.size(); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		c[y].resize(texture.t[y].size());
		for (size_t x = 0; x < c[y].size(); x++)
			c[y][x] = ((texture.t[y][x].c != ' ' && texture.t[y][x].f.a >= alphaThreshold) ? true : false);
	}
}

void KTech::Collider::ByTextureBackground(const Texture& texture, uint8_t alphaThreshold, uint8_t _type)
{
    simple = false;
    type = _type;
    pos_r = texture.pos_r;

	c.resize(texture.t.size());
	for (size_t y = 0; y < texture.t.size(); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		c[y].resize(texture.t[y].size());
		for (size_t x = 0; x < c[y].size(); x++)
			c[y][x] = (texture.t[y][x].b.a >= alphaThreshold ? true : false);
	}
}

// [Simple/Complex] Get the size of the texture.
KTech::UPoint KTech::Collider::GetSize() const
{
	if (simple)
		return size;
	else
	{
		UPoint result(0, c.size());
		for (const std::vector<bool>& row : c)
			if (row.size() > result.x)
				result.x = row.size();
		return result;
	}
}
