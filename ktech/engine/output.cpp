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

#include "../utility/internals.hpp"
#include "../utility/keys.hpp"
#include "../utility/rgbcolors.hpp"
#include "../basic/cell.hpp"
#include "../basic/upoint.hpp"
#include "../engine/engine.hpp"

#include <iostream>

void KTech::Output::Log(const std::string& p_text, RGB p_color)
{
	static uint32_t logIndex = 0;
	std::cout << "\033[38;2;" << std::to_string(p_color.r) << ';' << std::to_string(p_color.g) << ';' << std::to_string(p_color.b) << 'm' << logIndex << "] " << p_text << "\033[m\n" << std::flush;
	logIndex++;
}

void KTech::Output::PrintStartupNotice(const std::string& p_title, const std::string& p_years, const std::string& p_author, const std::string& p_programName) const
{
	// Clear the terminal
	std::cout << "\033[H\033[3J\033[2J";

	// Title
	std::cout << p_title << "\n";
	// Copyright (C)
	std::cout << "Copyright (C) " << p_years << " " << p_author << "\n\n";
	// Copying
	std::cout << p_programName << " is free software: you can redistribute it and/or modify\n";
	std::cout << "it under the terms of the GNU General Public License as published by\n";
	std::cout << "he Free Software Foundation, either version 3 of the License, or\n";
	std::cout << "any later version.\n\n";
	// No warranty
	std::cout << p_programName << " is distributed in the hope that it will be useful,\n";
	std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
	std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n";
	std::cout << "GNU General Public License for more details.\n\n";
	// GPL link
	std::cout << "You should have received a copy of the GNU General Public License\n";
	std::cout << "along with this program. If not, see <https://www.gnu.org/licenses/>.\n\n\n";

	// Wait for user input
	std::cout << "Read the legal notices, and then press the 'return' key (enter) to proceed..." << std::flush;
	while (engine->input.input != Keys::return_ && engine->running) {}

	if (!engine->running)
	{
		exit(0); // exit the thread completely to stop game loading prior to entering game loop
	}
}

void KTech::Output::Clear()
{
	std::fill(m_image.begin(), m_image.end(), Cell(' ', RGB(0, 0, 0), RGB(0, 0, 0)));
}

void KTech::Output::Draw(const std::vector<Cell>& p_sourceImage, UPoint p_resolution, Point p_position, UPoint p_start, UPoint p_end, uint8_t p_alpha)
{
	// Default the rectangle
	if (p_end.x == 0)
	{
		p_end.x = p_resolution.x;
	}
	if (p_end.y == 0)
	{
		p_end.y = p_resolution.y;
	}

	// ITERATE
	for (size_t yDst = (p_position.y < 0 ? 0 : p_position.y), ySrc = p_start.y; yDst < resolution.y && ySrc < p_end.y; yDst++, ySrc++)
	{
		for (size_t xDst = (p_position.x < 0 ? 0 : p_position.x), xSrc = p_start.x; xDst < resolution.x && xSrc < p_end.x; xDst++, xSrc++)
		{
			// DRAW character
			m_image[resolution.x * yDst + xDst].c = p_sourceImage[p_resolution.x * ySrc + xSrc].c;

			// DRAW foreground
			RGBA tempRGBA;
			if (BakeRGBA(tempRGBA, RGBA(p_sourceImage[p_resolution.x * ySrc + xSrc].f, p_alpha)))
			{
				DrawBakedToRGB(m_image[resolution.x * yDst + xDst].f, tempRGBA); // Draw given color if alpha isn't 0
			}
			else
			{
				m_image[resolution.x * yDst + xDst].f = RGB(0, 0, 0); // Draw black directly if alpha is 0
			}

			// DRAW foreground
			if (BakeRGBA(tempRGBA, RGBA(p_sourceImage[p_resolution.x * ySrc + xSrc].b, p_alpha)))
			{
				DrawBakedToRGB(m_image[resolution.x * yDst + xDst].b, tempRGBA); // Draw given color if alpha isn't 0
			}
			else
			{
				m_image[resolution.x * yDst + xDst].b = RGB(0, 0, 0); // Draw black directly if alpha is 0
			}
		}
	}
}

void KTech::Output::Draw(const std::vector<CellA>& p_sourceImage, UPoint p_resolution, Point p_position, UPoint p_start, UPoint p_end, uint8_t p_alpha)
{
	// Default the rectangle
	if (p_end.x == 0)
	{
		p_end.x = p_resolution.x;
	}
	if (p_end.y == 0)
	{
		p_end.y = p_resolution.y;
	}

	// ITERATE
	for (size_t yDst = (p_position.y < 0 ? 0 : p_position.y), ySrc = p_start.y; yDst < resolution.y && ySrc < p_end.y; yDst++, ySrc++)
	{
		for (size_t xDst = (p_position.x < 0 ? 0 : p_position.x), xSrc = p_start.x; xDst < resolution.x && xSrc < p_end.x; xDst++, xSrc++)
		{
			// DRAW character
			char charToDraw = p_sourceImage[p_resolution.x * ySrc + xSrc].c;
			if (DetermineCharacter(charToDraw))
			{
				m_image[resolution.x * yDst + xDst].c = charToDraw;
			}

			// DRAW foreground
			RGBA tempRGBA;
			if (BakeRGBAWith(tempRGBA, p_sourceImage[p_resolution.x * ySrc + xSrc].f, p_alpha))
			{
				DrawBakedToRGB(m_image[resolution.x * yDst + xDst].f, tempRGBA);
			}

			// DRAW background
			if (BakeRGBAWith(tempRGBA, p_sourceImage[p_resolution.x * ySrc + xSrc].b, p_alpha))
			{
				DrawBakedToRGB(m_image[resolution.x * yDst + xDst].b, tempRGBA);
			}
		}
	}
}

void KTech::Output::Print()
{
	/*
		This is a very old function.

		I recently improved its maintainability by separating it into `PopulateForegroundColor()`, `PopulateBackgroundColor()`, `PopulateCharacter()` and `PopulateEndOfLine()` `Output` member functions, as well as `GetThirdDigit()`, `GetSecondDigit()` and `GetFirstDigit()` in `internals.hpp`.

		However, I did not change its logic, and it's design and suggested optimizations are questionable. As I previously had an idea for a new printing approach, I may create a new printing method in the future, rather than enhance this current function.
	*/

	// Get terminal size
#ifdef _WIN32
	GetConsoleScreenBufferInfo(m_stdoutHandle, &m_csbi);
	m_terminalSize.ws_col = m_csbi.srWindow.Right - m_csbi.srWindow.Left + 1;
	m_terminalSize.ws_row = m_csbi.srWindow.Bottom - m_csbi.srWindow.Top + 1;
#else
	ioctl(fileno(stdout), TIOCGWINSZ, &m_terminalSize);
#endif

	// Write the image to stringImage
	size_t l{0};
	RGB background{0, 0, 0};
	RGB foreground{0, 0, 0};

	// "&& y < size.ws_row" - fit into the terminal, in the case that it is too small
	// "&& printRequests == 1" - stop working on a print if there is a newer print request
	for (size_t y = 0; y < resolution.y && y < m_terminalSize.ws_row; y++)
	{
		if (y != 0) {
			m_stringImage[l] = '\n';
			l++;
		}
		// Foreground
		foreground = m_image[resolution.x * y].f;
		PopulateForegroundColor(l, foreground);
		// Background
		background = m_image[resolution.x * y].b;
		PopulateBackgroundColor(l, background);
		// Character
		PopulateCharacter(l, resolution.x * y);
		for (size_t x = 1; x < resolution.x && x < m_terminalSize.ws_col; x++)
		{
			// foreground
			if (m_image[resolution.x * y + x].c != ' ' && m_image[resolution.x * y + x].f != foreground)
			{
				foreground = m_image[resolution.x * y + x].f;
				PopulateForegroundColor(l, foreground);
			}
			// background
			if (m_image[resolution.x * y + x].b != background)
			{
				background = m_image[resolution.x * y + x].b;
				PopulateBackgroundColor(l, background);
			}
			// Character
			PopulateCharacter(l, resolution.x * y + x);
		}
		PopulateEndOfLine(l);
	}
	// Move cursor to top-left corner ("ESC[H") and print
#if defined(_WIN32) && !defined(DEBUG)
	// Hide cursor ("ESC[?25l") since it always reappears on resize due to a Windows console bug
	std::cout << "\033[H\033[?25l" << m_stringImage.substr(0, l) << std::flush;
#else
	std::cout << "\033[H" << m_stringImage.substr(0, l) << std::flush;
#endif
}

auto KTech::Output::ShouldRenderThisTick() -> bool
{
	if (engine->input.m_changedThisTick
		|| engine->memory.m_changedThisTick
		|| engine->time.m_changedThisTick
		|| engine->time.ticksCounter == 0)
	{
		engine->input.m_changedThisTick = false;
		engine->memory.m_changedThisTick = false;
		engine->time.m_changedThisTick = false;
		return true;
	}
	return false;
}

auto KTech::Output::ShouldPrintThisTick() const -> bool
{
	winsize tempTerminalSize;
#ifdef _WIN32
	GetConsoleScreenBufferInfo(m_stdoutHandle, &m_csbi);
	tempTerminalSize.ws_col = m_csbi.srWindow.Right - m_csbi.srWindow.Left + 1;
	tempTerminalSize.ws_row = m_csbi.srWindow.Bottom - m_csbi.srWindow.Top + 1;
#else
	ioctl(fileno(stdout), TIOCGWINSZ, &tempTerminalSize);
#endif
	if (tempTerminalSize.ws_row != m_terminalSize.ws_row || tempTerminalSize.ws_col != m_terminalSize.ws_col)
	{
		// Clear terminal to remove potential tears
		std::cout << "[3J\033[2J" << std::flush;
		return true;
	}
	return false;
}

KTech::Output::Output(Engine* p_engine, KTech::UPoint p_imageResolution)
	: engine(p_engine),
	resolution(p_imageResolution),
	m_image(p_imageResolution.x * p_imageResolution.y, Cell(' ', RGBColors::black, RGBColors::black)),
	m_stringImage(p_imageResolution.y * 3 + p_imageResolution.x * p_imageResolution.y * printSequenceLength, ' ')
{
#ifdef _WIN32
	m_stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(m_stdoutHandle, &m_oldMode);
	SetConsoleMode(m_stdoutHandle, m_oldMode
		| ENABLE_VIRTUAL_TERMINAL_PROCESSING // "Virtual processing"
		| ENABLE_PROCESSED_OUTPUT // "Output processing"
	);
#endif

#ifdef DEBUG
	// Switch to alternative buffer (don't hide cursor for breaking with a debugger like GDB)
	std::cout << "\033[?1049h";
#else
	// Switch to alternative buffer and hide cursor
	std::cout << "\033[?1049h\033[?25l";
#endif
}

KTech::Output::~Output()
{
	// Show cursor, and disable alternative buffer (return to previous terminal)
	std::cout << "\033[?25h\033[?1049l" << std::flush;

	for (std::string& out : outputOnQuit)
	{
		std::cout << out;
	}

#ifdef _WIN32
	SetConsoleMode(m_stdoutHandle, m_oldMode);
#endif
}

void KTech::Output::PopulateForegroundColor(size_t& p_dst, const RGB& p_src)
{
	m_stringImage[p_dst++] = '\033';
	m_stringImage[p_dst++] = '[';
	m_stringImage[p_dst++] = '3';
	m_stringImage[p_dst++] = '8';
	m_stringImage[p_dst++] = ';';
	m_stringImage[p_dst++] = '2';
	m_stringImage[p_dst++] = ';';
	m_stringImage[p_dst++] = GetThirdDigit(p_src.r);
	m_stringImage[p_dst++] = GetSecondDigit(p_src.r);
	m_stringImage[p_dst++] = GetFirstDigit(p_src.r);
	m_stringImage[p_dst++] = ';';
	m_stringImage[p_dst++] = GetThirdDigit(p_src.g);
	m_stringImage[p_dst++] = GetSecondDigit(p_src.g);
	m_stringImage[p_dst++] = GetFirstDigit(p_src.g);
	m_stringImage[p_dst++] = ';';
	m_stringImage[p_dst++] = GetThirdDigit(p_src.b);
	m_stringImage[p_dst++] = GetSecondDigit(p_src.b);
	m_stringImage[p_dst++] = GetFirstDigit(p_src.b);
	m_stringImage[p_dst++] = 'm';
}

void KTech::Output::PopulateBackgroundColor(size_t& p_dst, const RGB& p_src)
{
	m_stringImage[p_dst++] = '\033';
	m_stringImage[p_dst++] = '[';
	m_stringImage[p_dst++] = '4';
	m_stringImage[p_dst++] = '8';
	m_stringImage[p_dst++] = ';';
	m_stringImage[p_dst++] = '2';
	m_stringImage[p_dst++] = ';';
	m_stringImage[p_dst++] = GetThirdDigit(p_src.r);
	m_stringImage[p_dst++] = GetSecondDigit(p_src.r);
	m_stringImage[p_dst++] = GetFirstDigit(p_src.r);
	m_stringImage[p_dst++] = ';';
	m_stringImage[p_dst++] = GetThirdDigit(p_src.g);
	m_stringImage[p_dst++] = GetSecondDigit(p_src.g);
	m_stringImage[p_dst++] = GetFirstDigit(p_src.g);
	m_stringImage[p_dst++] = ';';
	m_stringImage[p_dst++] = GetThirdDigit(p_src.b);
	m_stringImage[p_dst++] = GetSecondDigit(p_src.b);
	m_stringImage[p_dst++] = GetFirstDigit(p_src.b);
	m_stringImage[p_dst++] = 'm';
}

void KTech::Output::PopulateCharacter(size_t& p_dst, size_t p_src)
{
	m_stringImage[p_dst++] = (m_image[p_src].c >= ' ' && m_image[p_src].c <= '~') ? m_image[p_src].c : '?';
}

void KTech::Output::PopulateEndOfLine(size_t& p_dst)
{
	m_stringImage[p_dst++] = '\033';
	m_stringImage[p_dst++] = '[';
	m_stringImage[p_dst++] = 'm';
}