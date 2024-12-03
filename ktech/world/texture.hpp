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

#include <filesystem>
#include <string>
#include <vector>

/*!
	A `CellA`-based sprite.

	`Object`s and `Widget`s have a vector of these `Texture`s (`Object::m_textures` and `Widget::m_textures`, respectively), which represents their visual appearance. `Texture`s are used to render `Object`s and `Widget`s in `Camera::Render()` and `UI::Render()`, respectively.

	Similarly to `Collider`, there are 2 forms of `Texture`s: "simple" and "complex".
	- Simple `Texture`s are just efficient uniform rectangles, which take the least amount of processing and memory. So, you should prefer using simple `Texture`s.
	- Complex `Texture`s are 2D bitmaps of `CellA`s which allow them to hold a detailed shape (in contrary to a uniform rectangle). This makes them the least efficient in terms of processing and memory. So, minimize your use of complex `Texture`s to when you need such detailed shapes.

	`Texture` has various functions to design itself. These functions can be "chained", because they all return a self-reference. Also note that some of these functions have an override that accepts a new a relative position to set. Usually you will first call one of the overrides that sets a relative position, and then continue using the ones that don't to further design the `Texture`.
*/
struct KTech::Texture
{
	bool m_active = true; //!< Activation status: `true` means enabled. `false` means disabled, and will be skipped in rendering.

	bool m_simple; //!< `true` means simple, `false` means complex.
	CellA m_value; //!< Uniform value (applies only to simple `Texture`s).
	Point m_rPos; //!< Position relative to the parent `Object` or `Widget`.
	UPoint m_size; //!< Rectangle size (used in both simple and complex `Texture`s).
	std::vector<CellA> m_t; //!< 1D vector of the 2D bitmap (used only in complex `Texture`s).

	auto Simple(UPoint size, CellA value) -> Texture&;
	auto Simple(UPoint size, CellA value, Point relativePosition) -> Texture&;
	auto Rectangle(UPoint size, CellA value) -> Texture&;
	auto Rectangle(UPoint size, CellA value, Point relativePosition) -> Texture&;
	auto File(const std::filesystem::path& filePath) -> Texture&;
	auto File(const std::filesystem::path& filePath, Point relativePosition) -> Texture&;
	auto Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background) -> Texture&;
	auto Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background, Point relativePosition) -> Texture&;
	auto Null() -> Texture&;
	auto Null(Point relativePosition) -> Texture&;

	auto operator()(size_t x, size_t y) -> CellA&;
	auto operator()(size_t x, size_t y) const -> const CellA&;

	// `newValue` - only fills the new cells.
	auto Resize(UPoint size, CellA newValue = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0))) -> Texture&;
	auto SetCell(CellA value) -> Texture&;
	auto SetForeground(RGBA value) -> Texture&;
	auto SetBackground(RGBA value) -> Texture&;
	auto SetCharacter(char value) -> Texture&;
	auto SetForegroundAlpha(uint8_t value) -> Texture&;
	auto SetBackgroundAlpha(uint8_t value) -> Texture&;
	auto SetAlpha(uint8_t value) -> Texture&;
	auto ReplaceCharacter(char oldValue, char newValue) -> Texture&;

	void ExportToFile(const std::filesystem::path& filePath) const;
	void Print();
};