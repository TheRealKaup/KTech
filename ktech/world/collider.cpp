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

void KTech::Collider::Simple(UPoint p_size, uint8_t p_type, Point p_relativePosition)
{
    m_simple = true;
    m_type = p_type;
    m_rPos = p_relativePosition;
    m_size = p_size;
}

void KTech::Collider::Write(const std::vector<std::string>& p_stringVector, uint8_t p_type, Point p_relativePosition)
{
    m_simple = false;
    m_type = p_type;
    m_rPos = p_relativePosition;
	// Get size
	m_size = UPoint(0, p_stringVector.size());
	for (const std::string& row : p_stringVector)
	{
		if (row.size() > m_size.x)
		{
			m_size.x = row.size();
		}
	}
	// Apply size
	m_c.resize(m_size.x * m_size.y);
	// Read from strings
	for (size_t y = 0; y < m_size.y; y++)
	{
		for (size_t x = 0; x < m_size.x && x < p_stringVector[y].size(); x++)
		{
			m_c[m_size.x * y + x] = p_stringVector[y][x] != ' ';
		}
	}
}

void KTech::Collider::ByTextureCharacter(const Texture& p_texture, uint8_t p_type)
{
    m_simple = false;
    m_type = p_type;
    m_rPos = p_texture.m_rPos;
	// Get size
	m_size = p_texture.m_size;
	// Apply size
	m_c.resize(p_texture.m_t.size());
	// Read from texture
	for (size_t i = 0; i < m_c.size(); i++)
	{
		m_c[i] = (p_texture.m_t[i].c != ' ');
	}
}

void KTech::Collider::ByTextureBackground(const Texture& p_texture, uint8_t p_alphaThreshold, uint8_t p_type)
{
    m_simple = false;
    m_type = p_type;
    m_rPos = p_texture.m_rPos;
	// Get size
	m_size = p_texture.m_size;
	// Apply size
	m_c.resize(p_texture.m_t.size());
	// Read from texture
	for (size_t i = 0; i < m_c.size(); i++)
	{
		m_c[i] = (p_texture.m_t[i].b.a >= p_alphaThreshold);
	}
}