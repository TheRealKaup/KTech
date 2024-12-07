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

#include "engine.hpp"

/*!
	@var Engine::running

	States whether the game loop should be running or not.

	Your game loop should continue iterating as long as this variable is true, and should exit if it's false. For example:

	@code{.cpp}
	// Game loop
	while (engine.running)
	{
		// Game loop iteration ...
	}
	@endcode

	@see `Engine::Quit()`
*/

/*!
	Construct the engine components.

	The engine components are your game's dependencies. For example, without `Memory`, you don't have anywhere to register your world structures at.

	As part of that, a couple of noticeable things will happen to the terminal on construction:

	- `Input` will set some terminal settings, and start waiting for user inputs.
	- `Output` will move to an alternative terminal buffer, leaving the terminal empty until it starts printing images.

	`Input` `Output` also correctly revert these changes in their destructors, as should a terminal application do.

	@param imageSize The size of the game's viewport.
	@param ticksPerSecondLimit The rate at which your game loop should iterate.
*/
KTech::Engine::Engine(UPoint imageSize, size_t ticksPerSecondLimit)
	: collision(*this), input(*this), output(*this, imageSize), time(*this, ticksPerSecondLimit) {}

/*!
	Set `Engine::running` to false.

	Simply sets `Engine::running` to false (signifying the game loop should break). Use this if you need a function to do this operation (e.g., to provide as a callback function), and don't want to write one yourself.
*/
void KTech::Engine::Quit()
{
	running = false;
}