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
#include "../basic/upoint.hpp"
#include "collision.hpp"
#include "input/input.hpp"
#include "memory.hpp"
#include "output.hpp"
#include "time.hpp"

/*!
	Complete engine containing all engine components.

	You need an `Engine` instance to do nearly everything in KTech (including creating world structures). Normally, you should have 1 instance of this class in your game. Make sure you don't accidentally create duplicate instances, as those could lead to unexpected behavior, especially in the `Input` engine componenet.

	However, the need for multiple instances of `Engine` in the same process is probable. For example, you might want 1 process managing multiple `Engine`s (that were made headless, or that their `Input` and `Output` componenets were redirected), each handling a lobby in a server. Who knows.
*/
class KTech::Engine
{
public:
	bool running = true;

	Collision collision; //!< `Collision` engine component.
	Input input; //!< `Input` engine component.
	Memory memory; //!< `Memory` engine component.
	Output output; //!< `Output` engine component.
	Time time; //!< `Time` engine component.

	Engine(UPoint imageSize, size_t ticksPerSecondLimit = 24);

	void Quit();
};