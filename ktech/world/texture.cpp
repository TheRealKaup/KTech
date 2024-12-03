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
	@see KTech::Texture::Simple(UPoint size, CellA value)
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
	@see KTech::Texture::Rectangle(UPoint size, CellA value)
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
	@see KTech::Texture::File(const std::filesystem::path& filePath)
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

/*!
	@fn KTech::Texture::Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background, Point relativePosition)
	@brief Write a complex `Texture` from a vector of strings, and set its relative position.
	@param relativePosition Relative position to set.
	@see KTech::Texture::Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background)
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
	@see KTech::Texture::Null()
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
	return m_t[m_size.x * p_y + p_x];
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
	return m_t[m_size.x * y + x];
}

/*!
	@fn KTech::Texture::Resize(UPoint size, CellA newValue)
	@brief Resize the `Texture` (whether simple or complex).

	@param size The new size.
	@param newValue Value for new cells.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::SetCell(CellA value)
	@brief Set all cell values (whether simple or complex).

	@param value New value.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::SetForeground(RGB value)
	@brief Set all foreground colors (whether simple or complex).

	@param value New value.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::SetBackground(RGB value)
	@brief Set all background colors (whether simple or complex).

	@param value New value.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::SetCharacter(char value)
	@brief Set all characters (whether simple or complex).

	@param value New value.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::SetForegroundAlpha(uint8_t value)
	@brief Set all foreground alpha channels (whether simple or complex).

	@param value New value.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::SetBackgroundAlpha(uint8_t value)
	@brief Set all background alpha channels (whether simple or complex).

	@param value New value.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::SetAlpha(uint8_t value)
	@brief Set all alpha channels (whether simple or complex).

	@param value New value.

	@return Self-reference for function chaining.
*/
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

/*!
	@fn KTech::Texture::ReplaceCharacter(char oldValue, char newValue)
	@brief Replace all instances of a character (whether simple or complex).

	@param oldValue Character to replace.
	@param newValue Character replacement.

	@return Self-reference for function chaining.
*/
auto KTech::Texture::ReplaceCharacter(char p_oldValue, char p_newValue) -> Texture&
{
	if (m_simple)
	{
		if (m_value.c == p_oldValue)
		{
			m_value.c = p_newValue;
		}
	}
	else
	{
		for (CellA& cell : m_t)
		{
			if (cell.c == p_oldValue)
			{
				cell.c = p_newValue;
			}
		}
	}
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