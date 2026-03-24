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

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"

#include <string>
#include <vector>

/*!
	@brief 2D physical space for `Object`s.

	`Object`s have a vector of these `Collider`s (`Object::m_colliders`), which represents their physical space. `Collider`s are used to process `Object` movement in `Collision::MoveObject()`.

	Similarly to `Texture`, there are 2 forms of `Collider`s: "simple" and "complex".
	- Simple colliders are just efficient filled rectangles, which take the least amount of processing and memory. So, you should prefer using simple `Collider`s.
	- Complex colliders are 2D bitmaps of `bool`s which allow them to hold a detailed shape (in contrary to a filled rectangle). This makes them the least efficient in terms of processing and memory. So, minimize your use of complex `Collider`s to when you need such detailed shapes.

	`Collider`s also have a "type", which you can define yourself (or first learn about) in `Collision::colliderTypes`.

	@see `Collision::colliderTypes`
*/
struct KTech::Collider
{
	bool m_active = true; //!< Activation status: `true` means enabled. `false` means disabled, and will be ignored in `Object` movement processing.

	bool m_simple; //!< `true` means simple, `false` means complex.
	uint8_t m_type; //!< `Collider` type, which determines collision results based on `Collision::colliderTypes`.
	Point m_rPos; //!< Relative position to the parent `Object`.
	UPoint m_size; //!< Rectangle size (used in both simple and complex forms).
	std::vector<bool> m_c; //!< 1D vector of the 2D bitmap (used only in complex form).

	/*!
		@fn `Collider::Simple`

		@brief Construct a simple `Collider` (filled rectangle).

		@param size Rectangle size.
		@param type Collider type, as defiend in `Collision::colliderTypes`.
		@param relativePosition Relative position to the parent Object.
	*/
	void Simple(UPoint size, uint8_t type, Point relativePosition = Point(0, 0));

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
	void Write(const std::vector<std::string>& stringVector, uint8_t type, Point relativePosition = Point(0, 0));

	/*!
		@fn Collider::ByTextureCharacter
		@brief Construct a `Collider` based on a `Texture`'s character values.

		@param texture The base `Texture`.
		@param type Collider type, as defiend in `Collision::colliderTypes`.
		@param excludedCharacter Cells with this character will not be turned on.
	*/
	void ByTextureCharacter(const Texture& texture, uint8_t type, char excludedCharacter = ' ');

	/*!
		@fn Collider::ByTextureBackground
		@brief Construct a `Collider` based on a `Texture`'s background values.

		@param texture The base `Texture`.
		@param type Collider type, as defiend in `Collision::colliderTypes`.
		@param alphaThreshold Cells with background alpha => this value will be turned on.
	*/
	void ByTextureBackground(const Texture& texture, uint8_t type, uint8_t alphaThreshold = 0);

	/*!
		@fn Collider::ByTextureForeground
		@brief Construct a `Collider` based on a `Texture`'s foreground values.

		@param texture The base `Texture`.
		@param type Collider type, as defiend in `Collision::colliderTypes`.
		@param alphaThreshold Cells with foreground alpha => this value will be turned on.
	*/
	void ByTextureForeground(const Texture& texture, uint8_t type, uint8_t alphaThreshold = 0);

	/*!
		@fn Collider::operator()(size_t x, size_t y)

		@brief Get a value from the 2D bitmap by-value (complex `Collider`s only).

		Useful if you don't want to convert the 2D position to the corresponding index in the bitmap vector (`Collider::m_c`).

		@param x X axis.
		@param y Y axis.

		@return The value of the 2D bitmap at the given location, by-value (not by-reference!)
	*/
	[[nodiscard]] auto operator()(size_t x, size_t y) const -> bool;
};
