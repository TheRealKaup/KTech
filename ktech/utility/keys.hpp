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

/*!
	@brief Various terminal input escape sequences.

	Very useful for `CallbacksGroup::RegisterCallback()`.
*/
namespace KTech::Keys
{
	constexpr char const* up = "\x1b[A";
	constexpr char const* down = "\x1b[B";
	constexpr char const* right = "\x1b[C";
	constexpr char const* left = "\x1b[D";
#ifdef _WIN32
	constexpr char const* return_ = "\r";
#else
	constexpr char const* return_ = "\n";
#endif
	constexpr char const* backspace = "\x7f";
	constexpr char const* escape = "\x1b";
	constexpr char const* pageUp = "\x1b[5~";
	constexpr char const* pageDown = "\x1b[6~";
	constexpr char const* home = "\x1b[H";
	constexpr char const* end = "\x1b[F";
	constexpr char const* insert = "\x1b[2~";
	constexpr char const* delete_ = "\x1b[3~";
	constexpr char const* f1 = "\x1bOP";
	constexpr char const* f2 = "\x1bOQ";
	constexpr char const* f3 = "\x1bOR";
	constexpr char const* f4 = "\x1bOS";
	constexpr char const* f5 = "\x1b[15~";
	constexpr char const* f6 = "\x1b[17~";
	constexpr char const* f7 = "\x1b[18~";
	constexpr char const* f8 = "\x1b[19~";
	constexpr char const* f9 = "\x1b[20~";
	constexpr char const* f10 = "\x1b[21~";
	// No F11 because of fullscreen
	constexpr char const* f12 = "\x1b[24~";
	constexpr char const* tab = "\x09";

	//! Inputs triggered with the "shift" modifier key.
	namespace Shift
	{
		constexpr char const* return_ = "\x1bOM";
		constexpr char const* delete_ = "\x1b[3;2~";
		constexpr char const* tab = "\x1b[Z";
	}

	//! Inputs triggered with the "control" modifier key.
	namespace Ctrl
	{
		constexpr char const* up = "\33[1;5A";
		constexpr char const* down = "\33[1;5B";
		constexpr char const* right = "\33[1;5C";
		constexpr char const* left = "\33[1;5D";
		constexpr char const* pageUp = "\x1b[5;5~";
		constexpr char const* pageDown = "\x1b[6;5~";
		constexpr char const* home = "\x1b[1;5H";
		constexpr char const* end = "\x1b[1;5F";
		constexpr char const* delete_ = "\x1b[3;5~";
		constexpr char const* backspace = "\x08";
	}

	//! Inputs triggered with the "alt" modifier key.
	namespace Alt
	{
		constexpr char const* up = "\33[1;3A";
		constexpr char const* down = "\33[1;3B";
		constexpr char const* right = "\33[1;3C";
		constexpr char const* left = "\33[1;3D";
		constexpr char const* return_ = "\x1b\x0a";
		constexpr char const* backspace = "\x1b\x7f";
		constexpr char const* escape = "\x1b\x1b";
		constexpr char const* pageUp = "\x1b[5;3~";
		constexpr char const* pageDown = "\x1b[6;3~";
		constexpr char const* home = "\x1b[1;3H";
		constexpr char const* end = "\x1b[1;3F";
		constexpr char const* insert = "\x1b[2;3~";
		constexpr char const* delete_ = "\x1b[3;3~";
	}

	//! Inputs triggered with the "contorl" and "alt" modifier key.
	namespace CtrlAlt
	{
		constexpr char const* up = "\33[1;7A";
		constexpr char const* down = "\33[1;7B";
		constexpr char const* right = "\33[1;7C";
		constexpr char const* left = "\33[1;7D";
		constexpr char const* pageUp = "\x1b[5;7~";
		constexpr char const* pageDown = "\x1b[6;7~";
		constexpr char const* home = "\x1b[1;7H";
		constexpr char const* end = "\x1b[1;7F";
		constexpr char const* insert = "\x1b[2;7~";
	}

	//! Inputs triggered with the "contorl" and "shift" modifier key.
	namespace CtrlShift
	{
		constexpr char const* up = "\33[1;6A";
		constexpr char const* down = "\33[1;6B";
		constexpr char const* right = "\33[1;6C";
		constexpr char const* left = "\33[1;6D";
	}
}