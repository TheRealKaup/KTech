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

#include "output.hpp"

#include "../utility/keys.hpp"
#include "../basic/cell.hpp"
#include "../basic/upoint.hpp"
#include "../engine/engine.hpp"

#include <iostream>

KTech::Output::Output(Engine* p_engine, KTech::UPoint p_imageSize)
	: engine(p_engine)
{
#ifndef DEBUG
	// Hide cursor
	std::cout << "\033[?25l";
#endif
	// Switch to alternative buffer
	std::cout << "\033[?1049h";
	// Size the image
	m_image.resize(p_imageSize.y);
	for (size_t y = 0; y < m_image.size(); y++)
		m_image[y].resize(p_imageSize.x);
	size_t maxStringSize = m_image.size() * 4; // reserved for '\n'
	for (size_t y = 0; y < m_image.size(); y++)
		maxStringSize += m_image[y].size() * 39; // reserved for characters
	if (maxStringSize == 0)
		return;
	if (m_stringImage.size() < maxStringSize)
		m_stringImage.resize(maxStringSize);
}

KTech::Output::~Output()
{
	// Show cursor, and disable alternative buffer (return to previous terminal)
	std::cout << "\033[?25h\033[?1049l" << std::flush;
	for (std::string& out : outputAfterQuit)
		std::cout << out;
}

void KTech::Output::Log(const std::string& p_text, RGB p_color)
{
	static uint32_t logIndex = 0;
	std::cout << "\033[38;2;" << std::to_string(p_color.r) << ';' << std::to_string(p_color.g) << ';' << std::to_string(p_color.b) << 'm' << logIndex << "] " << p_text << "\033[m" << std::endl << std::flush;
	logIndex++;
}

void KTech::Output::PrintStartupNotice(const std::string& p_title, const std::string& p_years, const std::string& p_author, const std::string& p_projectName)
{
	// Clear the terminal
	std::cout << "\033[H\033[3J\033[2J";

	// Title
	std::cout << p_title << "\n";
	// Copyright (C)
	std::cout << "Copyright (C) " << p_years << " " << p_author << "\n\n";
	// Copying
	std::cout << p_projectName << " is free software: you can redistribute it and/or modify\n";
	std::cout << "it under the terms of the GNU General Public License as published by\n";
	std::cout << "he Free Software Foundation, either version 3 of the License, or\n";
	std::cout << "any later version.\n\n";
	// No warranty
	std::cout << p_projectName << " is distributed in the hope that it will breturn_e useful,\n";
	std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
	std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n";
	std::cout << "GNU General Public License for more details.\n\n";
	// GPL link
	std::cout << "You should have received a copy of the GNU General Public License\n";
	std::cout << "along with this program. If not, see <https://www.gnu.org/licenses/>.\n\n\n";

	// Wait for user input
	std::cout << "Read the legal notices, and then press the 'return' key (enter) to proceed..." << std::flush;
	while (engine->input.input != Keys::return_ && engine->running);
	if (!engine->running)
		exit(0); // exit the thread completely
}

void KTech::Output::Clear()
{
	for (size_t y = 0; y < m_image.size(); y++)
		for (size_t x = 0; x < m_image[y].size(); x++)
			m_image[y][x] = Cell(' ', RGB(0, 0, 0), RGB(0, 0, 0));
}

void KTech::Output::Draw(const std::vector<std::vector<Cell>>& p_render, Point p_pos, uint16_t p_left, uint16_t p_top, uint16_t p_right, uint16_t p_bottom, uint8_t p_alpha)
{
	// Default the rectangle
	if (p_bottom == 0)
		p_bottom = p_render.size();
	if (p_right == 0)
		p_right = p_render[0].size();
	// Return if rectangle is invalid
	if (p_left >= p_right || p_top >= p_bottom)
		return;

	// Draw
	for (size_t yF = (p_pos.y < 0 ? 0 : p_pos.y), yR = p_top; yF < m_image.size() && yR < p_bottom; yF++, yR++)
	{
		for (size_t xF = (p_pos.x < 0 ? 0 : p_pos.x), xR = p_left; xF < m_image[yF].size() && xR < p_right; xF++, xR++)
		{
			m_image[yF][xF].c = p_render[yR][xR].c;
			//                   8 ->                 16 ->     + 8 ->                16 ->                8.
			m_image[yF][xF].f.r = (p_render[yR][xR].f.r * p_alpha + m_image[yF][xF].f.r * (255 - p_alpha)) / 255;
			m_image[yF][xF].f.g = (p_render[yR][xR].f.g * p_alpha + m_image[yF][xF].f.g * (255 - p_alpha)) / 255;
			m_image[yF][xF].f.b = (p_render[yR][xR].f.b * p_alpha + m_image[yF][xF].f.b * (255 - p_alpha)) / 255;
			m_image[yF][xF].b.r = (p_render[yR][xR].b.r * p_alpha + m_image[yF][xF].b.r * (255 - p_alpha)) / 255;
			m_image[yF][xF].b.g = (p_render[yR][xR].b.g * p_alpha + m_image[yF][xF].b.g * (255 - p_alpha)) / 255;
			m_image[yF][xF].b.b = (p_render[yR][xR].b.b * p_alpha + m_image[yF][xF].b.b * (255 - p_alpha)) / 255;
		}
	}
}

void KTech::Output::Print()
{
	// Get terminal size
	ioctl(fileno(stdout), TIOCGWINSZ, &m_terminalSize);

	// Obtain the maximum length for the stringImage
	if (m_image.size() == 0)
		return;

	// Resize the stringImage if needed
	size_t maxStringSize = m_image.size() * 3; // reserved for '\n'
	for (size_t y = 0; y < m_image.size(); y++)
		maxStringSize += m_image[y].size() * 39; // reserved for characters
	if (maxStringSize == 0)
		return;
	if (m_stringImage.size() < maxStringSize) // resize
		m_stringImage.resize(maxStringSize);

	// Write the image to stringImage
	size_t l = 0;
	uint8_t lfr = 0, lfg = 0, lfb = 0, lbr = 0, lbg = 0, lbb = 0;

	// "&& y < size.ws_row" - fit into the terminal, in the case that it is too small
	// "&& printRequests == 1" - stop working on a print if there is a newer print request
	for (size_t y = 0; y < m_image.size() && y < m_terminalSize.ws_row; y++)
	{
		if (y != 0) {
			m_stringImage[l] = '\n';
			l++;
		}
		// First char in the line (optimizations)
		lfr = m_image[y][0].f.r;
		lfg = m_image[y][0].f.g;
		lfb = m_image[y][0].f.b;

		m_stringImage[l] = '\033';
		m_stringImage[l + 1] = '[';
		m_stringImage[l + 2] = '3';
		m_stringImage[l + 3] = '8';
		m_stringImage[l + 4] = ';';
		m_stringImage[l + 5] = '2';
		m_stringImage[l + 6] = ';';
		m_stringImage[l + 7] = lfr / 100 + '0';
		m_stringImage[l + 8] = (lfr % 100) / 10 + '0';
		m_stringImage[l + 9] = lfr % 10 + '0';
		m_stringImage[l + 10] = ';';
		m_stringImage[l + 11] = lfg / 100 + '0';
		m_stringImage[l + 12] = (lfg % 100) / 10 + '0';
		m_stringImage[l + 13] = lfg % 10 + '0';
		m_stringImage[l + 14] = ';';
		m_stringImage[l + 15] = lfb / 100 + '0';
		m_stringImage[l + 16] = (lfb % 100) / 10 + '0';
		m_stringImage[l + 17] = lfb % 10 + '0';
		m_stringImage[l + 18] = 'm';
		lbr = m_image[y][0].b.r;
		lbg = m_image[y][0].b.g;
		lbb = m_image[y][0].b.b;
		m_stringImage[l + 19] = '\033';
		m_stringImage[l + 20] = '[';
		m_stringImage[l + 21] = '4';
		m_stringImage[l + 22] = '8';
		m_stringImage[l + 23] = ';';
		m_stringImage[l + 24] = '2';
		m_stringImage[l + 25] = ';';
		m_stringImage[l + 26] = lbr / 100 + '0';
		m_stringImage[l + 27] = (lbr % 100) / 10 + '0';
		m_stringImage[l + 28] = lbr % 10 + '0';
		m_stringImage[l + 29] = ';';
		m_stringImage[l + 30] = lbg / 100 + '0';
		m_stringImage[l + 31] = (lbg % 100) / 10 + '0';
		m_stringImage[l + 32] = lbg % 10 + '0';
		m_stringImage[l + 33] = ';';
		m_stringImage[l + 34] = lbb / 100 + '0';
		m_stringImage[l + 35] = (lbb % 100) / 10 + '0';
		m_stringImage[l + 36] = lbb % 10 + '0';
		m_stringImage[l + 37] = 'm';
		if (m_image[y][0].c >= ' ' && m_image[y][0].c <= '~')
			m_stringImage[l + 38] = m_image[y][0].c;
		else
			m_stringImage[l + 38] = '?';
		l += 39;
		for (size_t x = 1; x < m_image[y].size() && x < m_terminalSize.ws_col; x++)
		{
			// foreground
			if ((m_image[y][x].c != ' ') && (m_image[y][x].f.r != lfr || m_image[y][x].f.g != lfg || m_image[y][x].f.b != lfb))
			{
				lfr = m_image[y][x].f.r;
				lfg = m_image[y][x].f.g;
				lfb = m_image[y][x].f.b;
				m_stringImage[l] = '\033';
				m_stringImage[l + 1] = '[';
				m_stringImage[l + 2] = '3';
				m_stringImage[l + 3] = '8';
				m_stringImage[l + 4] = ';';
				m_stringImage[l + 5] = '2';
				m_stringImage[l + 6] = ';';
				m_stringImage[l + 7] = lfr / 100 + '0';
				m_stringImage[l + 8] = (lfr % 100) / 10 + '0';
				m_stringImage[l + 9] = lfr % 10 + '0';
				m_stringImage[l + 10] = ';';
				m_stringImage[l + 11] = lfg / 100 + '0';
				m_stringImage[l + 12] = (lfg % 100) / 10 + '0';
				m_stringImage[l + 13] = lfg % 10 + '0';
				m_stringImage[l + 14] = ';';
				m_stringImage[l + 15] = lfb / 100 + '0';
				m_stringImage[l + 16] = (lfb % 100) / 10 + '0';
				m_stringImage[l + 17] = lfb % 10 + '0';
				m_stringImage[l + 18] = 'm';
				l += 19;
			}
			// background
			if (m_image[y][x].b.r != lbr || m_image[y][x].b.g != lbg || m_image[y][x].b.b != lbb)
			{
				lbr = m_image[y][x].b.r;
				lbg = m_image[y][x].b.g;
				lbb = m_image[y][x].b.b;

				m_stringImage[l] = '\033';
				m_stringImage[l + 1] = '[';
				m_stringImage[l + 2] = '4';
				m_stringImage[l + 3] = '8';
				m_stringImage[l + 4] = ';';
				m_stringImage[l + 5] = '2';
				m_stringImage[l + 6] = ';';
				m_stringImage[l + 7] = lbr / 100 + '0';
				m_stringImage[l + 8] = (lbr % 100) / 10 + '0';
				m_stringImage[l + 9] = lbr % 10 + '0';
				m_stringImage[l + 10] = ';';
				m_stringImage[l + 11] = lbg / 100 + '0';
				m_stringImage[l + 12] = (lbg % 100) / 10 + '0';
				m_stringImage[l + 13] = lbg % 10 + '0';
				m_stringImage[l + 14] = ';';
				m_stringImage[l + 15] = lbb / 100 + '0';
				m_stringImage[l + 16] = (lbb % 100) / 10 + '0';
				m_stringImage[l + 17] = lbb % 10 + '0';
				m_stringImage[l + 18] = 'm';
				l += 19;
			}
			if (m_image[y][x].c >= ' ' && m_image[y][x].c <= '~')
				m_stringImage[l] = m_image[y][x].c;
			else
				m_stringImage[l] = '?';
			l++;
		}
		m_stringImage[l] = '\033';
		m_stringImage[l + 1] = '[';
		m_stringImage[l + 2] = 'm';
		l += 3;
	}
	std::cout << "\033[H\033[3J\033[2J" << m_stringImage.substr(0, l) << std::flush;
}

bool KTech::Output::ShouldRenderThisTick()
{
	if (engine->input.inputThisTick || engine->time.invokedThisTick || engine->memory.callChangedThisTick || engine->time.ticksCounter == 0)
	{
		engine->input.inputThisTick = false;
		engine->time.invokedThisTick = false;
		engine->memory.callChangedThisTick = false;
		return true;
	}
	return false;
}

bool KTech::Output::ShouldPrintThisTick()
{
	winsize tempTerminalSize;
	ioctl(fileno(stdout), TIOCGWINSZ, &tempTerminalSize);
	if (tempTerminalSize.ws_row != m_terminalSize.ws_row || tempTerminalSize.ws_col != m_terminalSize.ws_col)
		return true;
	return false;
}