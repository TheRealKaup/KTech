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

#include <string>
#include <termio.h>
#include <vector>

class KTech::Output
{
public:
	Output(Engine* const engine, KTech::UPoint imageSize);
	~Output();

	static void Log(const std::string& text, RGB color);

	void PrintStartupNotice(const std::string& title, const std::string& years, const std::string& author, const std::string& programName);
	// Clears the in-engine image, not the terminal.
	void Clear();
	void Draw(const std::vector<std::vector<Cell>>& image, Point position = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0, uint8_t alpha = 255);
	void Print();

	// If game loop is designed to render-on-demand, use this function to determine whether there is demand, that is, should the game loop
	// render, draw and print.
	bool ShouldRenderThisTick();
	// Returns true if the terminal resized this tick, which means the game loop should print even if it didn't render.
	bool ShouldPrintThisTick();

private:
	Engine* const engine;

	winsize m_terminalSize;
	std::vector<std::vector<Cell>> m_image;
	std::string m_stringImage;
};