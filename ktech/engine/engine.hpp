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
#include "audio.hpp"
#include "collision.hpp"
#include "io.hpp"
#include "memory.hpp"
#include "time.hpp"
#include "../basic/rgb.hpp"
#include "../basic/upoint.hpp"

// Core class, the user should create one of these, and shouldn't have to construct themselves 
// a system using the other namespaces themselves
class KTech::Engine
{
public:
	Collision collision = Collision(this);
	Audio audio;
	Time time;
	IO io;
	Memory memory;

	bool running = true;

	Engine(UPoint imageSize, int16_t tps = 24)
		: io(IO(imageSize, this)), time(Time(tps, running)) {}

	~Engine()
	{
		IO::Log("<Engine::~Engine()>", RGB(255, 0, 0));
	}
};