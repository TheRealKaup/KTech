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

class KTech::Engine
{
public:
	bool running = true;

	Collision collision;
	Input input;
	Memory memory;
	Output output;
	Time time;

	inline Engine(UPoint imageSize, int16_t ticksPerSecondLimit = 24)
		: collision(*this), input(*this), output(*this, imageSize), time(*this, ticksPerSecondLimit) {}

	inline void Quit()
	{
		running = false;
	}
};