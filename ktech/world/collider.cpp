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

#include "collider.hpp"

#include "texture.hpp"

#include <fstream>

void KTech::Collider::Simple(UPoint p_size, uint8_t p_type, Point p_pos)
{
    m_simple = true;
    m_type = p_type;
    m_rPos = p_pos;
    m_size = p_size;
}

bool KTech::Collider::File(const std::string& p_fileName, uint8_t p_type, Point p_pos)
{
    m_simple = false;
    m_type = p_type;
    m_rPos = p_pos;

	std::ifstream file(p_fileName);
	if (!file.is_open())
		return false;
	std::string line;

	for (size_t y = 0; std::getline(file, line); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		m_c.resize(m_c.size() + 1);
		m_c[y].resize(line.length());
		for (size_t x = 0; x < line.length(); x++)
			m_c[y][x] = line[x];
	}
	
	return true;
}

void KTech::Collider::Write(const std::vector<std::string>& p_stringVector, uint8_t p_type, Point p_pos)
{
    m_simple = false;
    m_type = p_type;
    m_rPos = p_pos;

	m_c.resize(p_stringVector.size());
	for (size_t y = 0; y < p_stringVector.size(); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		m_c[y].resize(p_stringVector[y].length());
		for (size_t x = 0; x < m_c[y].size(); x++)
			m_c[y][x] = p_stringVector[y][x] == ' ' ? false : true;
	}
}

void KTech::Collider::ByTextureCharacter(const Texture& p_texture, uint8_t p_alphaThreshold, uint8_t p_type)
{
    m_simple = false;
    m_type = p_type;
    m_rPos = p_texture.m_rPos;

	m_c.resize(p_texture.m_size.y);
	for (size_t y = 0; y < m_c.size(); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		m_c[y].resize(p_texture.m_size.x);
		for (size_t x = 0; x < m_c[y].size(); x++)
			m_c[y][x] = ((p_texture(x, y).c != ' ' && p_texture(x, y).f.a >= p_alphaThreshold) ? true : false);
	}
}

void KTech::Collider::ByTextureBackground(const Texture& p_texture, uint8_t p_alphaThreshold, uint8_t p_type)
{
    m_simple = false;
    m_type = p_type;
    m_rPos = p_texture.m_rPos;

	m_c.resize(p_texture.m_size.y);
	for (size_t y = 0; y < m_c.size(); y++)
	{
		// potentially broken if one of the values = 10 ('\n')
		m_c[y].resize(p_texture.m_size.x);
		for (size_t x = 0; x < m_c[y].size(); x++)
			m_c[y][x] = (p_texture(x, y).b.a >= p_alphaThreshold ? true : false);
	}
}

std::vector<bool>& KTech::Collider::operator[](size_t y)
{
	return m_c[y];
}

const std::vector<bool>& KTech::Collider::operator[](size_t y) const
{
	return m_c[y];
}

// [Simple/Complex] Get the size of the texture.
KTech::UPoint KTech::Collider::GetSize() const
{
	if (m_simple)
		return m_size;
	else
	{
		UPoint result(0, m_c.size());
		for (const std::vector<bool>& row : m_c)
			if (row.size() > result.x)
				result.x = row.size();
		return result;
	}
}