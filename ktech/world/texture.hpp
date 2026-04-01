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
#include "../basic/cella.hpp"
#include "../basic/point.hpp"
#include "../basic/rgba.hpp"
#include "../basic/upoint.hpp"

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

/*!
	@brief A `CellA`-based sprite.

	`Object`s and `Widget`s have a vector of these `Texture`s (`Object::m_textures` and `Widget::m_textures`, respectively), which represents their visual appearance. `Texture`s are used to render `Object`s and `Widget`s in `Camera::Render()` and `UI::Render()`, respectively.

	Similarly to `Collider`, there are 2 forms of `Texture`s: "simple" and "complex".
	- Simple `Texture`s are just efficient uniform rectangles, which take the least amount of processing and memory. So, you should prefer using simple `Texture`s.
	- Complex `Texture`s are 2D bitmaps of `CellA`s which allow them to hold a detailed shape (in contrary to a uniform rectangle). This makes them the least efficient in terms of processing and memory. So, minimize your use of complex `Texture`s to when you need such detailed shapes.

	`Texture` has various functions to design itself. These functions can be "chained", because they all return a self-reference. Also note that some of these functions have an override that accepts a new a relative position to set. Usually you will first call one of the overrides that sets a relative position, and then continue using the ones that don't to further design the `Texture`.
*/
struct KTech::Texture
{
	//! Activation status: `true` means enabled. `false` means disabled, and will be skipped in rendering.
	bool m_active = true;

	bool m_simple;			//!< `true` means simple, `false` means complex.
	CellA m_value;			//!< Uniform value (applies only to simple `Texture`s).
	Point m_rPos;			//!< Position relative to the parent `Object` or `Widget`.
	UPoint m_size;			//!< Rectangle size (used in both simple and complex `Texture`s).
	std::vector<CellA> m_t; //!< 1D vector of the 2D bitmap (used only in complex `Texture`s).

	/*!
		@fn KTech::Texture::Simple(UPoint size, CellA value)
		@brief Create a simple `Texture` (uniform rectangle).
		@param size Rectangle's size.
		@param value Rectangle's `CellA` value.
		@return Self-reference for function chaining.
	*/
	auto Simple(UPoint size, CellA value) -> Texture&;

	/*!
		@fn KTech::Texture::Simple(UPoint size, CellA value, Point relativePosition)
		@brief Create a simple `Texture` (uniform rectangle), and set its relative position.
		@param relativePosition Relative position to set.
		@param size Rectangle's size.
		@param value Rectangle's `CellA` value.
		@return Self-reference for function chaining.
	*/
	auto Simple(UPoint size, CellA value, Point relativePosition) -> Texture&;

	/*!
		@fn KTech::Texture::Rectangle(UPoint size, CellA value)
		@brief Create a complex `Texture` that is a uniform rectangle.

		Even though a simple `Texture` (created using `Texture::Simple()`) is capable of the same result at first glance, you can use this function to create a base for further modifications.

		@param size Rectangle's size.
		@param value Rectangle's uniform `CellA` value.
		@return Self-reference for function chaining.
	*/
	auto Rectangle(UPoint size, CellA value) -> Texture&;

	/*!
		@fn KTech::Texture::Rectangle(UPoint size, CellA value, Point relativePosition)
		@brief Create a complex `Texture` that is a uniform rectangle, and set its relative position.

		@param relativePosition Relative position to set.
		@param size Rectangle's size.
		@param value Rectangle's uniform `CellA` value.
		@return Self-reference for function chaining.

		@see `KTech::Texture::Rectangle(UPoint size, CellA value)`
	*/
	auto Rectangle(UPoint size, CellA value, Point relativePosition) -> Texture&;

	/*!
		@fn KTech::Texture::File(const std::filesystem::path& filePath)
		@brief Import a complex `Texture` from file.

		Such files can be created with <a href="https://github.com/TheRealKaup/TextureCreator">TextureCreator</a>, or with `Texture::Export()`.

		@param filePath Path to the texture file (`.ktecht`).

		@see <a href="https://github.com/TheRealKaup/TextureCreator">TextureCreator</a>
		@see `Texture::Export()`
		@return Self-reference for function chaining.
	*/
	auto File(const std::filesystem::path& filePath) -> Texture&;

	/*!
		@fn KTech::Texture::File(const std::filesystem::path& filePath, Point relativePosition)
		@brief Import a complex `Texture` from file, and set its relative position.

		@param relativePosition Relative position to set.
		@param filePath Path to the texture file (`.ktecht`).
		@return Self-reference for function chaining.

		@see `KTech::Texture::File(const std::filesystem::path& filePath)`
	*/
	auto File(const std::filesystem::path& filePath, Point relativePosition) -> Texture&;

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
	auto Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background) -> Texture&;

	/*!
		@fn KTech::Texture::Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background, Point relativePosition)
		@brief Write a complex `Texture` from a vector of strings, and set its relative position.

		@param relativePosition Relative position to set.
		@param stringVector Vector of strings that will be converted into the `Texture`'s characters.
		@param foreground Uniform foreground (character) color.
		@param background Uniform background color.

		@return Self-reference for function chaining.
	*/
	auto Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background, Point relativePosition)
		-> Texture&;

	/*!
		@brief Create a complex `Texture` that represents a missing `Texture`.

		Used by `Texture::File()` if it failed to open or read the given file.

		@return Self-reference for function chaining.
	*/
	auto Null() -> Texture&;

	/*!
		@fn KTech::Texture::Null(Point relativePosition)
		@brief Create a complex `Texture` that represents a missing `Texture`, and set its relative position.

		@param relativePosition Relative position to set.
		@return Self-reference for function chaining.

		@see `Texture::Null()`
	*/
	auto Null(Point relativePosition) -> Texture&;

	/*!
		@fn KTech::Texture::operator()(size_t x, size_t y)

		@brief Get a value from the 2D bitmap by-reference (complex `Texture`s only).

		Useful if you don't want to convert the 2D position into the corresponding index on the bitmap vector (`Texture::m_t`).

		@param x X axis.
		@param y Y axis.
		@return The value of the 2D bitmap at the given location, by-reference.
	*/
	auto operator()(size_t x, size_t y) -> CellA&;

	/*!
		@fn KTech::Texture::operator()(size_t x, size_t y) const

		@brief Get a value from the 2D bitmap by-reference (complex `Texture`s only).

		Useful if you don't want to convert the 2D position into the corresponding index on the bitmap vector (`Texture::m_t`).

		@param x X axis.
		@param y Y axis.

		@return The value of the 2D bitmap at the given location, by-const-reference.
	*/
	auto operator()(size_t x, size_t y) const -> const CellA&;

	/*!
		@fn KTech::Texture::Resize
		@brief Resize the `Texture` (whether simple or complex).

		@param size The new size.
		@param newValue Value for new cells. Does not affect simple `Texture`s.

		@return Self-reference for function chaining.
	*/
	auto Resize(UPoint size, CellA newValue = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0))) -> Texture&;

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
	auto Transform(const std::function<void(CellA&)>& operation, UPoint from = UPoint(0, 0), UPoint to = UPoint(0, 0))
		-> Texture&;

	/*!
		@fn KTech::Texture::ExportToFile(const std::filesystem::path& filePath)
		@brief Export `Texture` to a file (complex `Texture`s only).

		@param filePath Path to output the texture file (`.ktecht`).
	*/
	void ExportToFile(const std::filesystem::path& filePath) const;

	/*!
		@brief Print the `Texture` to the terminal (complex `Texture`s only).

		Can be useful in debugging.
	*/
	void Print() const;
};
