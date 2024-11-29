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

/*!
	Engine component responsible for outputting rendered images.
*/
class KTech::Output
{
public:
	/*!
		The size of the image buffer (viewport).
	*/
	const UPoint resolution;
	/*!
		Vector of strings to print when the game quits (specifically, in `Output::~Output()`).
	*/
	std::vector<std::string> outputOnQuit;

	/*!
		Print colored log text to the terminal.

		Simply `std::cout`'s and `std::flash`'s the given text. It has no unique functionality, and doesn't integrate with the output system.

		This function is really only useful when no other printing is being done, because `Output::Print()` will print over the logged text in the terminal.

		@param [in] text Text to print.
		@param [in] color Color for the outputted text.
	*/
	static void Log(const std::string& text, RGB color);

	/*!
		Print a copyright notice.

		The GPLv3 requires your "interactive user interface" (your game) to display "Appropriate Legal Notices", which are defined in the following excerpt:

		> An interactive user interface displays “Appropriate Legal Notices” to the extent that it includes a convenient and prominently visible feature that (1) displays an appropriate copyright notice, and (2) tells the user that there is no warranty for the work (except to the extent that warranties are provided), that licensees may convey the work under this License, and how to view a copy of this License. If the interface presents a list of user commands or options, such as a menu, a prominent item in the list meets this criterion.

		You can use this function (or your own implementation) to just print everything that's needed (appropriate copyright notice, warranty disclaimer, copying, and viewing a copy of the license). This function simplifies this by putting your parameter values into a prepared template. If your warranty status differs from what this function prints, you should of course print this on your own.

		If you want to have a "prominent item" in your menu to display Appropriate Legal Notices (whether in addition to or instead of printing at the start of your game), you can use the `AboutBox` widget to display the text, and `Switch` to hide and show it. This exact method is practiced in the "widgetstest" game example.

		@param [in] title Title of your project (name and brief description).
		@param [in] years Years where your project was released in.
		@param [in] author Your name.
		@param [in] programName Your project's name.

		Something like this will be printed:

		@code{.unparsed}
		{title}
		Copyright (C) {years} {author}

		{programName} is free software: you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		he Free Software Foundation, either version 3 of the License, or
		any later version.

		{programName} is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program. If not, see <https://www.gnu.org/licenses/>.
		@endcode
	*/
	void PrintStartupNotice(const std::string& title, const std::string& years, const std::string& author, const std::string& programName) const;

	/*!
		Clear the image buffer.

		Doesn't clear the terminal; that is done by `Output::Print()` just before printing the new image.

		Use this function if you need to draw your image on a blank sheet. For example, if you simply draw a fully-opaque `Camera` image each frame, you can probably skip this part, because `Camera` images completely override old image buffer cells. However, if you are drawing a `UI` image, you will want to first call this function, because `UI` images don't always completely override old image buffer cells. This is because `Camera` images are `Cell`-based, while `UI` images are `CellA`-based.

		It's recommended to use this function in conjunction with `Output::ShouldRenderThisTick()`.

		@see `Output::ShouldRenderThisTick()`
	*/
	void Clear();

	/*!
		Draw a `Cell`-based image .

		This function is used for `Camera` images. It is supposed to completely override old image buffer cells, unless `alpha` is not set to max (255).

		It's recommended to use this function in conjunction with `Output::ShouldRenderThisTick()`.

		@param [in] sourceImage Image to draw.
		@param [in] resolution Resolution of the given `sourceImage`.
		@param [in] position Where to draw the image on the internal image buffer.
		@param [in] start From where to draw `sourceImage`.
		@param [in] end Until where to draw `sourceImage`.
		@param [in] alpha Opacity to draw at.

		@see `Output::ShouldRenderThisTick()`
	*/
	void Draw(const std::vector<Cell>& sourceImage, UPoint resolution, Point position = Point(0, 0), UPoint start = UPoint(0, 0), UPoint end = UPoint(0, 0), uint8_t alpha = std::numeric_limits<uint8_t>::max());

	/*!
		Draw a `CellA`-based image .

		This function is used for `UI` images. It is supposed to draw over old image buffer cells rather than completely override them (like `Camera` images do). This behavior is desired so a `UI` image can be drawn like a heads-up display, for example, on top of a fully-opaque `Camera` image.

		It's recommended to use this function in conjunction with `Output::ShouldRenderThisTick()`.

		@param [in] sourceImage Image to draw.
		@param [in] resolution Resolution of the given `sourceImage`.
		@param [in] position Where to draw the image on the internal image buffer.
		@param [in] start From where to draw `sourceImage`.
		@param [in] end Until where to draw `sourceImage`.
		@param [in] alpha Opacity to draw at.

		@see `Output::ShouldRenderThisTick()`
	*/
	void Draw(const std::vector<CellA>& sourceImage, UPoint resolution, Point position = Point(0, 0), UPoint start = UPoint(0, 0), UPoint end = UPoint(0, 0), uint8_t alpha = std::numeric_limits<uint8_t>::max());

	/*!
		Print the internal image buffer.

		Call this function after preparing the image buffer using `Output::Clear()` and `Output::Draw()` in whatever way that fits your needs.

		It's recommended to use this function in conjunction with `Output::ShouldRenderThisTick()` and `Output::ShouldPrintThisTick()`.

		@see `Output::ShouldRenderThisTick()`, `Output::ShouldPrintThisTick()`
	*/
	void Print();

	/*!
		Check whether things changed and require a new render.

		The `Input`, `Memory` and `Time` engine components remember whether any user callback function returned true throughout the last tick. If any did, it means something has changed (like the position of an `Object` or the appearance of a `Texture`). In that case, this function will return true. This allows to "render-on-demand", i.e., avoiding rendering when certainly nothing has changed.

		For best performance, use this function in conjunction with `Output::ShouldRenderThisTick()` (there is an example at its documentation entry):

		@return Whether you should render and draw again.

		@see `Output::ShouldPrintThisTick()`
	*/
	[[nodiscard]] auto ShouldRenderThisTick() -> bool;

	/*!
		Check whether the terminal changed and requires a new print.

		The `Output` engine component keeps track of the terminal size, because if it changes, the engine needs to adapt. If that happened, this function returns true, telling you that the image buffer should be printed again.

		For best performance, use this function in conjunction with `Output::ShouldRenderThisTick()`. Here's an example:

		@code{.cpp}
		if (engine.output.ShouldRenderThisTick())
		{
			// Render, draw and print
			ui.Render();
			engine.output.Clear();
			ui.Render();
			engine.output.Print();
		}
		else if (engine.output.ShouldPrintThisTick())
		{
			// Just print
			engine.output.Print();
		}
		@endcode

		@return Whether you should render and draw again.
		@see `Output::ShouldRenderThisTick()`
	*/
	[[nodiscard]] auto ShouldPrintThisTick() const -> bool;

private:
	Engine& engine;
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

	Output(Engine& engine, UPoint imageResolution);
	~Output();

	void PopulateForegroundColor(size_t& dst, const RGB& src);
	void PopulateBackgroundColor(size_t& dst, const RGB& src);
	void PopulateCharacter(size_t& dst, size_t src);
	void PopulateEndOfLine(size_t& dst);

	friend class Engine;
};