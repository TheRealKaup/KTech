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

#include <limits>
#include <string>
#ifdef _WIN32
#include <Windows.h>
#undef RGB
#else
#include <termio.h>
#endif
#include <vector>

class KTech::Output
{
public:
	const UPoint resolution;
	std::vector<std::string> outputAfterQuit;

	Output(Engine* engine, UPoint imageResolution);
	~Output();

	static void Log(const std::string& text, RGB color);

	void PrintStartupNotice(const std::string& title, const std::string& years, const std::string& author, const std::string& programName) const;
	// Clears the in-engine image, not the terminal.
	void Clear();
	void Draw(const std::vector<Cell>& sourceImage, UPoint resolution, Point position = Point(0, 0), UPoint start = UPoint(0, 0), UPoint end = UPoint(0, 0), uint8_t alpha = std::numeric_limits<uint8_t>::max());
	void Draw(const std::vector<CellA>& sourceImage, UPoint resolution, Point position = Point(0, 0), UPoint start = UPoint(0, 0), UPoint end = UPoint(0, 0), uint8_t alpha = std::numeric_limits<uint8_t>::max());
	void Print();

	// If game loop is designed to render-on-demand, use this function to determine whether there is demand, that is, should the game loop
	// render, draw and print.
	auto ShouldRenderThisTick() -> bool;
	// Returns true if the terminal resized this tick, which means the game loop should print even if it didn't render.
	[[nodiscard]] auto ShouldPrintThisTick() const -> bool;

private:
	Engine* const engine;
#if _WIN32
	HANDLE m_stdoutHandle;
	DWORD m_oldMode;
	CONSOLE_SCREEN_BUFFER_INFO m_csbi;
	struct winsize {
		size_t ws_row;
		size_t ws_col;
	} m_terminalSize;
#else
	winsize m_terminalSize;
#endif
	std::vector<Cell> m_image;
	std::string m_stringImage;
	static constexpr size_t printSequenceLength = 39;

	void PopulateForegroundColor(size_t& dst, const RGB& src);
	void PopulateBackgroundColor(size_t& dst, const RGB& src);
	void PopulateCharacter(size_t& dst, size_t src);
	void PopulateEndOfLine(size_t& dst);
};