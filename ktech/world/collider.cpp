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

/*!
	@fn `Collider::Simple`

	@brief Construct a simple `Collider` (filled rectangle).

	@param size Rectangle size.
	@param type Collider type, as defiend in `Collision::colliderTypes`.
	@param relativePosition Relative position to the parent Object.
*/
void KTech::Collider::Simple(UPoint p_size, uint8_t p_type, Point p_relativePosition)
{
    m_simple = true;
    m_type = p_type;
    m_rPos = p_relativePosition;
    m_size = p_size;
}

/*!
	@fn `Collider::Write`

	@brief Construct a complex `Collider` (2D bitmap).

	@param stringVector The 2D bitmap representation in a vector of `std::string`s. If a character is a space (' '), the collider will not exist in the corresponding part of the resulting bitmap. Otherwise, the collider will exist there.
	@param type Collider type, as defiend in `Collision::colliderTypes`.
	@param relativePosition Relative position to the parent Object.

	Example:

	@code{.cpp}
	Collider::Write(
		{
			" # ",
			"###",
			"# #"
		}, 1, Point(0, 0)
	);
	@endcode

	Will construct a `Collider` that corresponds to the `Texture`:

	@code{.cpp}
	Texture::Write(
		{
			" O ",
			"/|\\",
			"/ \\"
		}, RGBA( 255, 255, 0, 255 ), RGBAColors::transparent, Point(0, 0)
	);
	@endcode
*/
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

//! @brief This function is undocumented because it's planned to change (see GitHub issue #128).
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

//! @brief This function is undocumented because it's planned to change (see GitHub issue #128).
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

/*!
	@fn Collider::operator()(size_t x, size_t y)

	@brief Get a value from the 2D bitmap by-value (complex `Collider`s only).

	Useful if you don't want to convert the 2D position to the corresponding index in the bitmap vector (`Collider::m_c`).

	@param x X axis.
	@param y Y axis.

	@return The value of the 2D bitmap at the given location, by-value (not by-reference!)
*/
auto KTech::Collider::operator()(size_t p_x, size_t p_y) const -> bool
{
	return m_c[m_size.x * p_y + p_x];
}