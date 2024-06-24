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

#include <string>
#include <vector>

/*!
	\brief 2D physical space
	\details Can be "simple" or "complex". Complex `Collider`s store an array of `bool` values, and simple `Collider`s store a singular `bool` which makes them a uniform rectangle. Complex `Collider`s are more capable, while simple `Collider`s are significantly faster to process and require less memory.
	\ingroup world
*/
struct KTech::Collider
{
	//! Whether to process this `Collider`
	bool m_active = true;

	/*! \name Physical space
		It's very recommended to use the constructor functions rather than alter these members directly
		@{
	*/
	//! Relative position
	Point m_rPos;
	//! If true, this `Collider` is simple. If false, this `Collider` is complex
	bool m_simple;
	UPoint m_size;
	//! Non-coalescing bitmap of `bool`s, only used in complex `Collider`s
	std::vector<uint8_t> m_c;
	//!@}

	//! Determines collision result with other `Collider`s. Can be chosen to be one of the types according to `Collision::colliderTypes`
	uint8_t m_type;

	/*! \name Constructors
		These functions initialize member variables, and should be called when the `Collider` is created or when you wish to recreate it
		@{
	*/
	//! Creates a simple `Collider`
	void Simple(UPoint size, uint8_t type, Point relativePosition = Point(0, 0));
	/*! \brief Creates a complex `Collider`
		\param[in] stringVector Can be written like `{" # ", "###", "# #"}`, which will make a corresponding `Collider` to the example in `Texture::Write()`
	*/
	void Write(const std::vector<std::string>& stringVector, uint8_t type, Point relativePosition = Point(0, 0));
	/*! \brief Creates a complex `Collider` corresponding with the characters of the given `Texture`
		\param[in] texture Any non-space `CellA` will lead to a filled `Collider` cell. Relative position is copied from it
	*/
	void ByTextureCharacter(const Texture& texture, uint8_t type);
	/*! \brief Creates a complex `Collider` corresponding with the foreground colors of the given `Texture`
		\param[in] texture Any `CellA` with background alpha channel higher or equal to `alphaThreshold` will lead to a filled `Collider` cell. Relative position is copied from it
	*/
	void ByTextureBackground(const Texture& texture, uint8_t alphaThreshold, uint8_t type);
	//!@}

	uint8_t& operator()(size_t x, size_t y);
	const uint8_t& operator()(size_t x, size_t y) const;
};