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

struct KTech::Texture
{
	bool m_active = true;

	bool m_simple;
	CellA m_value;
	Point m_rPos;
	UPoint m_size;
	std::vector<CellA> m_t;

	auto Simple(UPoint size, CellA value) -> Texture&;
	auto Simple(UPoint size, CellA value, Point relativePosition) -> Texture&;
	auto Rectangle(UPoint size, CellA value) -> Texture&;
	auto Rectangle(UPoint size, CellA value, Point relativePosition) -> Texture&;
	auto File(const std::filesystem::path& filePath) -> Texture&;
	auto File(const std::filesystem::path& filePath, Point relativePosition) -> Texture&;
	auto Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background) -> Texture&;
	auto Write(const std::vector<std::string>& stringVector, RGBA foreground, RGBA background, Point relativePosition) -> Texture&;
	// Creates "missing texture"
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
	// For debugging
	void Print();
};