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

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"
#include "../basic/rgba.hpp"
#include "../basic/cella.hpp"

#include <string>
#include <vector>

/*!
	\brief 2D `CellA`-based image
	\details Can be "simple" or "complex". Complex `Texture`s store an array of `CellA` values, and simple `Texture`s store a singular `CellA` which makes them a uniform rectangle. Complex `Texture`s are more visually capable, while simple `Texture`s are significantly faster to render and require less memory.
	\ingroup world
*/
struct KTech::Texture
{
	bool m_active = true; //!< Whether to render this `Texture`

	/*! \name Appearance
		It's very recommended to use the constructor and modifier functions rather than alter these members directly
		@{
	*/
	//! Relative position
	Point m_rPos;
	//! If true, this `Texture` is simple. If false, this `Texture` is complex
	bool m_simple;
	UPoint m_size;
	//! Uniform `CellA` value, only used in simple `Texture`s
	CellA m_value;
	//! Bitmap of `CellA`s, only used in complex `Texture`s
	std::vector<CellA> m_t; 
	//!@}

	/*! \name Constructors
		Can be chained, usually before modifier functions. These functions initialize member variables, and should be called when the `Texture` is created or when you wish to recreate it
		\param[in] relativePosition Optional, if given changes current position and redirects to base constructor 
		@{
	*/
	//! Creates a simple `Texture`
	Texture& Simple(UPoint size, CellA value);
	Texture& Simple(UPoint size, CellA value, Point relativePosition);
	//! Creates a uniform complex `Texture`, useful for later altering
	Texture& Rectangle(UPoint size, CellA value);
	Texture& Rectangle(UPoint size, CellA value, Point relativePosition);
	//! Creates a complex `Texture` by reading from a file
	Texture& File(const std::string& fileName);
	Texture& File(const std::string& fileName, Point relativePosition);
	/*! \brief Creates a complex `Texture` with uniform foreground and background colors, but not characters
		\param[in] stringVector Can be written like `{" o ", "/|\", "/ \\"}`, which will make a small figure
	*/
	Texture& Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background);
	Texture& Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background, Point relativePosition);
	//!@}

	CellA& operator()(size_t x, size_t y);
	const CellA& operator()(size_t x, size_t y) const;

	/*! \name Modifiers
		Modify already created `Texture`. You would usually use these after calling one of the constructor functions. They automatically check whether the `Texture` is simple or complex, and operate accordingly
	*/
	//!@{
	/*! \brief Resizes and does not override already existing cells
		\param[in] value Used to fill new cells (meaning, unused if the given size is smaller than the current size)
	*/
	Texture& Resize(UPoint size, CellA value = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)));
	//! Sets all of the cells
	Texture& SetCell(CellA value);
	//! Sets all foreground colors
	Texture& SetForeground(RGBA value);
	//! Sets all background colors
	Texture& SetBackground(RGBA value);
	//! Sets all characters
	Texture& SetCharacter(char value);
	//! Sets all foreground alpha channels
	Texture& SetForegroundAlpha(uint8_t value);
	//! Sets all background alpha channels
	Texture& SetBackgroundAlpha(uint8_t value);
	//! Sets all alpha channels
	Texture& SetAlpha(uint8_t value);
	/*! \brief Replaces characters
		\param[in] oldValue Characters that are this value will be replaced
		\param[in] newValue Said characters will be set to this
	*/
	Texture& ReplaceCharacter(char oldValue, char newValue);
	//!@}
	
	//! Write the complex `Texture` values to a file (`m_size` and `m_t`)
	void ExportToFile(const std::string& fileName) const;
	//! (For debugging) print the `Texture` directly to the terminal
	void Print();
};