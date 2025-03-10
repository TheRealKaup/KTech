/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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

#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <limits>

/*!
	@fn KTech::Texture::Simple(UPoint size, CellA value)
	@brief Create a simple `Texture` (uniform rectangle).
	@param size Rectangle's size.
	@param value Rectangle's `CellA` value.
	@return Self-reference for function chaining.
*/
auto KTech::Texture::Simple(UPoint p_size, CellA p_value) -> Texture&
{
	m_simple = true;
	m_size = p_size;
	m_value = p_value;
	return *this;
}

/*!
	@fn KTech::Texture::Simple(UPoint size, CellA value, Point relativePosition)
	@brief Create a simple `Texture` (uniform rectangle), and set its relative position.
	@param relativePosition Relative position to set.
	@param size Rectangle's size.
	@param value Rectangle's `CellA` value.
	@return Self-reference for function chaining.
*/
auto KTech::Texture::Simple(UPoint p_size, CellA p_value, Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Simple(p_size, p_value);
}

/*!
	@fn KTech::Texture::Rectangle(UPoint size, CellA value)
	@brief Create a complex `Texture` that is a uniform rectangle.

	Even though a simple `Texture` (created using `Texture::Simple()`) is capable of the same result at first glance, you can use this function to create a base for further modifications.

	@param size Rectangle's size.
	@param value Rectangle's uniform `CellA` value.
	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::Rectangle(UPoint size, CellA value, Point relativePosition)
	@brief Create a complex `Texture` that is a uniform rectangle, and set its relative position.

	@param relativePosition Relative position to set.
	@param size Rectangle's size.
	@param value Rectangle's uniform `CellA` value.
	@return Self-reference for function chaining.

	@see `KTech::Texture::Rectangle(UPoint size, CellA value)`
*/
auto KTech::Texture::Rectangle(UPoint p_size, CellA p_value, Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Rectangle(p_size, p_value);
}

/*!
	@fn KTech::Texture::File(const std::filesystem::path& filePath)
	@brief Import a complex `Texture` from file.

	Such files can be created with <a href="https://github.com/TheRealKaup/TextureCreator">TextureCreator</a>, or with `Texture::Export()`.

	@param filePath Path to the texture file (`.ktecht`).

	@see <a href="https://github.com/TheRealKaup/TextureCreator">TextureCreator</a>
	@see `Texture::Export()`
	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::File(const std::filesystem::path& filePath, Point relativePosition)
	@brief Import a complex `Texture` from file, and set its relative position.

	@param relativePosition Relative position to set.
	@param filePath Path to the texture file (`.ktecht`).
	@return Self-reference for function chaining.

	@see `KTech::Texture::File(const std::filesystem::path& filePath)`
*/
auto KTech::Texture::File(const std::filesystem::path& p_filePath, Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return File(p_filePath);
}

/*!
	@fn KTech::Texture::Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background)
	@brief Write a complex `Texture` from a vector of strings.

	Creates a complex `Texture` with different characters but uniform foreground and background colors. For example:

	@code{.cpp}
	m_textures[0].Write(
		{
			" O ",
			"/|\\",
			"/ \\"
		}, RGBA( 255, 255, 0, 255 ), RGBAColors::transparent, Point(0, 0)
	);
	@endcode

	@param stringVector Vector of strings that will be converted into the `Texture`'s characters.
	@param foreground Uniform foreground (character) color.
	@param background Uniform background color.

	@return Self-reference for function chaining.
*/
auto KTech::Texture::Write(const std::vector<std::string>& p_stringVector, RGBA p_foreground, RGBA p_background) -> Texture&
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

/*!
	@fn KTech::Texture::Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background, Point relativePosition)
	@brief Write a complex `Texture` from a vector of strings, and set its relative position.

	@param relativePosition Relative position to set.
	@param stringVector Vector of strings that will be converted into the `Texture`'s characters.
	@param foreground Uniform foreground (character) color.
	@param background Uniform background color.

	@return Self-reference for function chaining.
*/
auto KTech::Texture::Write(const std::vector<std::string>& p_stringVector, RGBA p_foreground, RGBA p_background, Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Write(p_stringVector, p_foreground, p_background);
}

/*!
	@brief Create a complex `Texture` that represents a missing `Texture`.

	Used by `Texture::File()` if it failed to open or read the given file.

	@return Self-reference for function chaining.
*/
auto KTech::Texture::Null() -> Texture&
{
	Resize(UPoint(2, 2));
	m_t[0] = CellA(' ', RGBAColors::transparent, RGBAColors::black);
	m_t[1] = CellA(' ', RGBAColors::transparent, RGBAColors::magenta);
	m_t[2] = CellA(' ', RGBAColors::transparent, RGBAColors::magenta);
	m_t[3] = CellA(' ', RGBAColors::transparent, RGBAColors::black);
	return *this;
}

/*!
	@fn KTech::Texture::Null(Point relativePosition)
	@brief Create a complex `Texture` that represents a missing `Texture`, and set its relative position.

	@param relativePosition Relative position to set.
	@return Self-reference for function chaining.

	@see `Texture::Null()`
*/
auto KTech::Texture::Null(Point p_relativePosition) -> Texture&
{
	m_rPos = p_relativePosition;
	return Null();
}

/*!
	@fn KTech::Texture::operator()(size_t x, size_t y)

	@brief Get a value from the 2D bitmap by-reference (complex `Texture`s only).

	Useful if you don't want to convert the 2D position into the corresponding index on the bitmap vector (`Texture::m_t`).

	@param x X axis.
	@param y Y axis.
	@return The value of the 2D bitmap at the given location, by-reference.
*/
auto KTech::Texture::operator()(size_t p_x, size_t p_y) -> CellA&
{
	return m_t[(m_size.x * p_y) + p_x];
}

/*!
	@fn KTech::Texture::operator()(size_t x, size_t y) const

	@brief Get a value from the 2D bitmap by-reference (complex `Texture`s only).

	Useful if you don't want to convert the 2D position into the corresponding index on the bitmap vector (`Texture::m_t`).

	@param x X axis.
	@param y Y axis.

	@return The value of the 2D bitmap at the given location, by-const-reference.
*/
auto KTech::Texture::operator()(size_t x, size_t y) const -> const CellA&
{
	return m_t[(m_size.x * y) + x];
}

/*!
	@fn KTech::Texture::Resize
	@brief Resize the `Texture` (whether simple or complex).

	@param size The new size.
	@param newValue Value for new cells. Does not affect simple `Texture`s.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::Transform
	@brief Transform the `Texture` using a operation callback function (whether simple or complex).

	@param[in] operation	Callback function that receives a `CellA` (old value) and returns a `CellA` (new value).
	@param[in] from 		Top-left corner of area to transform. Default: (0, 0).
	@param[in] to 			Bottom-right corner of area to transform. Default: size of `Texture`.

	Example that reverses the background colors of a `Texture`:

	\code{.cpp}
	Example missing!
	\endcode

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::ExportToFile(const std::filesystem::path& filePath)
	@brief Export `Texture` to a file (complex `Texture`s only).

	@param filePath Path to output the texture file (`.ktecht`).
*/
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

/*!
	@brief Print the `Texture` to the terminal (complex `Texture`s only).

	Can be useful in debugging.
*/
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
	std::cout << std::flush;
}