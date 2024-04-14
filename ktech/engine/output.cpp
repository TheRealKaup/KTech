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

#include "io.hpp"
#include "../misc/rgbcolors.hpp"
#include "../misc/keys.hpp"
#include "../basic_structs/upoint.hpp"
#include "../basic_structs/cell.hpp"
#include "../basic_structs/cella.hpp"
#include "../engine/engine.hpp"

#include <iostream>

void KTech::IO::Draw(const std::vector<std::vector<CellA>>& render, Point pos, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, uint8_t alpha)
{
	// Default the rectangle
	if (bottom == 0)
		bottom = render.size();
	if (right == 0)
		right = render[0].size();
	// Return if rectangle is invalid
	if (left >= right || top >= bottom)
		return;

	// To avoid repeating the same calculation
	uint8_t tempAlpha;

	// Draw
	for (size_t yF = (pos.y < 0 ? 0 : pos.y), yR = top; yF < image.size() && yR < bottom; yF++, yR++)
	{
		for (size_t xF = (pos.x < 0 ? 0 : pos.x), xR = left; xF < image[yF].size() && xR < right; xF++, xR++)
		{
			if (render[yR][xR].c != ' ')
			{
				image[yF][xF].c = render[yR][xR].c;
				tempAlpha = render[yR][xR].f.a * alpha / 255;
				//                   8 ->                 16 ->     + 8 ->                16 ->                8.
				image[yF][xF].f.r = (render[yR][xR].f.r * tempAlpha + image[yF][xF].f.r * (255 - tempAlpha)) / 255;
				image[yF][xF].f.g = (render[yR][xR].f.g * tempAlpha + image[yF][xF].f.g * (255 - tempAlpha)) / 255;
				image[yF][xF].f.b = (render[yR][xR].f.b * tempAlpha + image[yF][xF].f.b * (255 - tempAlpha)) / 255;
			}
			tempAlpha = render[yR][xR].b.a * alpha / 255;
			//                   8 ->                 16 ->     + 8 ->                16 ->                8.
			image[yF][xF].b.r = (render[yR][xR].b.r * tempAlpha + image[yF][xF].b.r * (255 - tempAlpha)) / 255;
			image[yF][xF].b.g = (render[yR][xR].b.g * tempAlpha + image[yF][xF].b.g * (255 - tempAlpha)) / 255;
			image[yF][xF].b.b = (render[yR][xR].b.b * tempAlpha + image[yF][xF].b.b * (255 - tempAlpha)) / 255;
		}
	}

}

void KTech::IO::Print()
{
	// Get terminal size
	ioctl(fileno(stdout), TIOCGWINSZ, &terminalSize);

	// Obtain the maximum length for the stringImage
	if (image.size() == 0)
		return;

	// Resize the stringImage if needed
	size_t maxStringSize = image.size() * 3; // reserved for '\n'
	for (size_t y = 0; y < image.size(); y++)
		maxStringSize += image[y].size() * 39; // reserved for characters
	if (maxStringSize == 0)
		return;
	if (stringImage.size() < maxStringSize) // resize
		stringImage.resize(maxStringSize);

	// Write the image to stringImage
	size_t l = 0;
	uint8_t lfr = 0, lfg = 0, lfb = 0, lbr = 0, lbg = 0, lbb = 0;

	// "&& y < size.ws_row" - fit into the terminal, in the case that it is too small
	// "&& printRequests == 1" - stop working on a print if there is a newer print request
	for (size_t y = 0; y < image.size() && y < terminalSize.ws_row; y++)
	{
		if (y != 0) {
			stringImage[l] = '\n';
			l++;
		}
		// First char in the line (optimizations)
		lfr = image[y][0].f.r;
		lfg = image[y][0].f.g;
		lfb = image[y][0].f.b;

		stringImage[l] = '\033';
		stringImage[l + 1] = '[';
		stringImage[l + 2] = '3';
		stringImage[l + 3] = '8';
		stringImage[l + 4] = ';';
		stringImage[l + 5] = '2';
		stringImage[l + 6] = ';';
		stringImage[l + 7] = lfr / 100 + '0';
		stringImage[l + 8] = (lfr % 100) / 10 + '0';
		stringImage[l + 9] = lfr % 10 + '0';
		stringImage[l + 10] = ';';
		stringImage[l + 11] = lfg / 100 + '0';
		stringImage[l + 12] = (lfg % 100) / 10 + '0';
		stringImage[l + 13] = lfg % 10 + '0';
		stringImage[l + 14] = ';';
		stringImage[l + 15] = lfb / 100 + '0';
		stringImage[l + 16] = (lfb % 100) / 10 + '0';
		stringImage[l + 17] = lfb % 10 + '0';
		stringImage[l + 18] = 'm';
		lbr = image[y][0].b.r;
		lbg = image[y][0].b.g;
		lbb = image[y][0].b.b;
		stringImage[l + 19] = '\033';
		stringImage[l + 20] = '[';
		stringImage[l + 21] = '4';
		stringImage[l + 22] = '8';
		stringImage[l + 23] = ';';
		stringImage[l + 24] = '2';
		stringImage[l + 25] = ';';
		stringImage[l + 26] = lbr / 100 + '0';
		stringImage[l + 27] = (lbr % 100) / 10 + '0';
		stringImage[l + 28] = lbr % 10 + '0';
		stringImage[l + 29] = ';';
		stringImage[l + 30] = lbg / 100 + '0';
		stringImage[l + 31] = (lbg % 100) / 10 + '0';
		stringImage[l + 32] = lbg % 10 + '0';
		stringImage[l + 33] = ';';
		stringImage[l + 34] = lbb / 100 + '0';
		stringImage[l + 35] = (lbb % 100) / 10 + '0';
		stringImage[l + 36] = lbb % 10 + '0';
		stringImage[l + 37] = 'm';
		if (image[y][0].c >= ' ' && image[y][0].c <= '~')
			stringImage[l + 38] = image[y][0].c;
		else
			stringImage[l + 38] = '?';
		l += 39;
		for (size_t x = 1; x < image[y].size() && x < terminalSize.ws_col; x++)
		{
			// foreground
			if ((image[y][x].c != ' ') && (image[y][x].f.r != lfr || image[y][x].f.g != lfg || image[y][x].f.b != lfb))
			{
				lfr = image[y][x].f.r;
				lfg = image[y][x].f.g;
				lfb = image[y][x].f.b;
				stringImage[l] = '\033';
				stringImage[l + 1] = '[';
				stringImage[l + 2] = '3';
				stringImage[l + 3] = '8';
				stringImage[l + 4] = ';';
				stringImage[l + 5] = '2';
				stringImage[l + 6] = ';';
				stringImage[l + 7] = lfr / 100 + '0';
				stringImage[l + 8] = (lfr % 100) / 10 + '0';
				stringImage[l + 9] = lfr % 10 + '0';
				stringImage[l + 10] = ';';
				stringImage[l + 11] = lfg / 100 + '0';
				stringImage[l + 12] = (lfg % 100) / 10 + '0';
				stringImage[l + 13] = lfg % 10 + '0';
				stringImage[l + 14] = ';';
				stringImage[l + 15] = lfb / 100 + '0';
				stringImage[l + 16] = (lfb % 100) / 10 + '0';
				stringImage[l + 17] = lfb % 10 + '0';
				stringImage[l + 18] = 'm';
				l += 19;
			}
			// background
			if (image[y][x].b.r != lbr || image[y][x].b.g != lbg || image[y][x].b.b != lbb)
			{
				lbr = image[y][x].b.r;
				lbg = image[y][x].b.g;
				lbb = image[y][x].b.b;

				stringImage[l] = '\033';
				stringImage[l + 1] = '[';
				stringImage[l + 2] = '4';
				stringImage[l + 3] = '8';
				stringImage[l + 4] = ';';
				stringImage[l + 5] = '2';
				stringImage[l + 6] = ';';
				stringImage[l + 7] = lbr / 100 + '0';
				stringImage[l + 8] = (lbr % 100) / 10 + '0';
				stringImage[l + 9] = lbr % 10 + '0';
				stringImage[l + 10] = ';';
				stringImage[l + 11] = lbg / 100 + '0';
				stringImage[l + 12] = (lbg % 100) / 10 + '0';
				stringImage[l + 13] = lbg % 10 + '0';
				stringImage[l + 14] = ';';
				stringImage[l + 15] = lbb / 100 + '0';
				stringImage[l + 16] = (lbb % 100) / 10 + '0';
				stringImage[l + 17] = lbb % 10 + '0';
				stringImage[l + 18] = 'm';
				l += 19;
			}
			if (image[y][x].c >= ' ' && image[y][x].c <= '~')
				stringImage[l] = image[y][x].c;
			else
				stringImage[l] = '?';
			l++;
		}
		stringImage[l] = '\033';
		stringImage[l + 1] = '[';
		stringImage[l + 2] = 'm';
		l += 3;
	}
	std::cout << "\033[H\033[3J\033[2J" << stringImage.substr(0, l) << std::flush;
	
	for (size_t y = 0; y < image.size(); y++)
		for (size_t x = 0; x < image[y].size(); x++)
			image[y][x] = Cell(' ', RGB(0, 0, 0), RGB(0, 0, 0));
}

void KTech::IO::PrintStartupNotice(const std::string& title, const std::string& years, const std::string author, const std::string nameOfProject)
{
	// Clear the terminal
	std::cout << "\033[H\033[3J\033[2J";

	// Title
	std::cout << title << "\n";
	// Copyright (C)
	std::cout << "Copyright (C) " << years << " " << author << "\n\n";
	// Copying
	std::cout << nameOfProject << " is free software: you can redistribute it and/or modify\n";
	std::cout << "it under the terms of the GNU General Public License as published by\n";
	std::cout << "he Free Software Foundation, either version 3 of the License, or\n";
	std::cout << "any later version.\n\n";
	// No warranty
	std::cout << nameOfProject << " is distributed in the hope that it will be useful,\n";
	std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
	std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n";
	std::cout << "GNU General Public License for more details.\n\n";
	// GPL link
	std::cout << "You should have received a copy of the GNU General Public License\n";
	std::cout << "along with this program. If not, see <https://www.gnu.org/licenses/>.\n\n\n";

	// Wait for user input
	std::cout << "Read the legal notices, and then press the 'return' key (enter) to proceed..." << std::flush;
	while (input != Keys::return_ && engine->running);
	if (!engine->running)
		exit(0); // exit the thread completely
}

void KTech::IO::Log(const std::string& text, RGB color)
{
	static uint32_t logIndex = 0;
	std::cout << "\033[38;2;" << std::to_string(color.r) << ';' << std::to_string(color.g) << ';' << std::to_string(color.b) << 'm' << logIndex << "] " << text << "\033[m" << std::endl << std::flush;
	// std::cout << logIndex << "] " << text << std::endl << std::flush;
	logIndex++;
}

KTech::IO::IO(KTech::UPoint imageSize, Engine* engine) : engine(engine)
{
	// (OUTPUT) Hide cursor and enable alternative buffer (the "save screen" and "restore screen" options aren't preferable, alternative buffer makes more sense).
	std::cout << "\033[?25l\033[?1049h";
	
	// (INPUT) Set terminal attributes
	tcgetattr(0, &oldTerminalAttributes);
	termios terminalAttributes = oldTerminalAttributes;
	terminalAttributes.c_lflag &= ~ICANON; // Disable canonical mode
	terminalAttributes.c_lflag &= ~ECHO; // Disable echo
	terminalAttributes.c_lflag &= ~ISIG; // Disable signals
	terminalAttributes.c_cc[VMIN] = 1; // Blocking read
	terminalAttributes.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &terminalAttributes);

	// (OUTPUT) Size the image
	image.resize(imageSize.y);
	for (size_t y = 0; y < image.size(); y++)
		image[y].resize(imageSize.x);
	size_t maxStringSize = image.size() * 4; // reserved for '\n'
	for (size_t y = 0; y < image.size(); y++)
		maxStringSize += image[y].size() * 39; // reserved for characters
	if (maxStringSize == 0)
		return;
	if (stringImage.size() < maxStringSize)
		stringImage.resize(maxStringSize);

	// (INPUT) Create input loop
	t_inputLoop = std::thread(std::bind(&IO::Loop, this));
}

KTech::IO::~IO()
{
	Log("<IO::~IO> Returning terminal attributes, showing cursor, disabling alternative buffer, and detaching input loop thread.", RGBColors::green);
	// Return to the old terminal attributes
	tcsetattr(0, TCSANOW, &oldTerminalAttributes);
	
	// Show cursor, and disable alternative buffer (return to previous terminal)
	std::cout << "\033[?25h\033[?1049l" << std::flush;
	
	// Detach so in the case that the game quit regardless of player input,
	// the input loop thread would end.
	t_inputLoop.detach();
}