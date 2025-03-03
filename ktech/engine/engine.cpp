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

#include "engine.hpp"

/*!
	@var Engine::running
	@brief States whether the game loop should be running or not.

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
	@var Engine::noGameLoopMode
	@brief States whether the engine is in "no-game-loop mode" or not.

	You can optionally set this mode to true in `Engine::Engine()`.

	"No-game-loop mode" changes some of the behavior of the `Input` and `Output` engine components in order to facilitate writing a test program. For example, if you want to isolate testing of `Texture`s, `Object` movement, etc., in order to see how these features work, you can use this mode.

	This is what changes when no-game-loop mode is enabled (jargon warning):

	- Doesn't start input-loop thread.
	- Doesn't disable canonical mode (input will be received after the user enters a new line).
	- Doesn't disable echo (user input will be displayed while being typed).
	- Doesn't disable signals (Ctrl+C, Ctrl+D, etc., will work normally).
	- Doesn't reset cursor position on each `Output::Print()` (images will be printed bellow each other).
	- Doesn't enable alternative buffer.
	- In Windows exclusively:
		- Doesn't enable input as escape sequences.
		- Doesn't disable selecting text.
		- Doesn't switch to us-ascii input (code page 20127).

	The most significant change is the input-loop thread not starting, which is the thread that collects user input for later distribution by `Input::CallCallbacks()`: this change allows you to wait for user input yourself with something like `std::cin`. The rest of the changes mean the terminal will behave like a shell rather than a game. All of this allows you to write a test program without the burden of a game loop. [Tutorial chapter 5](https://github.com/TheRealKaup/KTech/blob/master/documentation/tutorial/tutorial.md#chapters) is an example of such program that enables this mode and doesn't use a game loop paradigm.

	@see `Engine::Engine()`
	@see [Tutorial chapter 5](https://github.com/TheRealKaup/KTech/blob/master/documentation/tutorial/tutorial.md#chapters)
*/

/*!
	@fn Engine::Engine
	@brief Construct the engine components.

	The engine components are your game's dependencies. For example, without `Memory`, you don't have anywhere to register your world structures at.

	As part of that, a couple of noticeable things will happen to the terminal on construction (unless `noGameLoopMode` is set to `true`):

	- `Input` will set some terminal settings, and start waiting for user inputs.
	- `Output` will move to an alternative terminal buffer, leaving the terminal empty until it starts printing images.

	`Input` and `Output` also correctly revert these changes in their destructors, as should a terminal application do.

	@param imageSize The size of the game's viewport.
	@param ticksPerSecondLimit The rate at which your game loop should iterate.
	@param noGameLoopMode `false`: normal IO behavior. `true`: change some IO behavior so it's possible to write a test program with a game loop. See `Engine::noGameLoopMode`.

	@see `Engine::noGameLoopMode`
*/
KTech::Engine::Engine(UPoint p_imageSize, size_t p_ticksPerSecondLimit, bool p_noGameLoopMode)
	: noGameLoopMode(p_noGameLoopMode), collision(*this), input(*this, p_noGameLoopMode), output(*this, p_imageSize, p_noGameLoopMode), time(*this, p_ticksPerSecondLimit) {}

/*!
	@brief Set `Engine::running` to false.

	Simply sets `Engine::running` to false (signifying the game loop should break). Use this if you need a function to do this operation (e.g., to provide as a callback function), and don't want to write one yourself.
*/
void KTech::Engine::Quit()
{
	running = false;
}