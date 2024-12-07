/*
	KTech's Tutorial, the official KTech tutorial taught by examples.
	Copyright (C) 2023-2024 Ethan Kaufman (AKA Kaup)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../../ktech/ktech.hpp"

auto main() -> int
{
	/*
		Here's a basic game world hierarchy. Change it however you'd like.

			map
			|________
			v       v
			layer   camera
			|
			v
			object
	*/

	// Engine
	KTech::Engine engine{KTech::UPoint(20, 20), 24};

	// Map
	KTech::Map map{engine};

	// Layer: add it to `map`.
	KTech::Layer layer{engine, map.m_id};

	// Object: add it to `layer`
	KTech::Object object{engine, layer.m_id, KTech::Point(5, 5)};
	// Give `object` some `Texture`s.
	object.m_textures.resize(2);
	object.m_textures[0].Simple(
		KTech::UPoint(3, 3),
		KTech::CellA('a', KTech::RGBAColors::black, KTech::RGBAColors::white)
		// Relative position not given: (0, 0) by default.
	);
	object.m_textures[1].Simple(
		KTech::UPoint(3, 3),
		KTech::CellA('b', KTech::RGBAColors::white, KTech::RGBAColors::black),
		KTech::Point(1, 1)
	);

	// Camera: add it to `Map`
	KTech::Camera camera{engine, map.m_id, KTech::Point(0, 0), engine.output.resolution};

	/*
		In KTech, you (the user) create your own game loop.

		The "game loop" is a loop that times and runs game iterations, which are known as "ticks".

		Each individual tick is usually responsible of (we are going to dive deeper into each of these tasks):

		- Updating the game state:
			- Distribute player inputs.
			- Call due timed invocations.
			- Call on-tick functions.
		- Outputting the game state:
			- Render.
			- Draw.
			- Print.
		- Waiting and continuing to the next tick.

		A game loop encloses these tasks (in an iteration; a tick) and lets them iterate as long as the game should keep running.
	*/

	/*
		After initializing our game (creating all our world structures and joining them together), we should enter our game loop, because we are ready for the game to start running:
	*/
	while (engine.running)								// (1)
	{
		// Call functions, that call other functions, that change things in your game:
		engine.input.CallCallbacks();					// (2)
		engine.time.CallInvocations();					// (3)
		engine.memory.CallOnTicks();					// (4)

		if (engine.output.ShouldRenderThisTick())		// (5)
		{
			// Draw and print if the game changed:
			camera.Render();							// (6)
			camera.Draw();								// (7)
			engine.output.Print();						// (8)
		}
		else if (engine.output.ShouldPrintThisTick())	// (9)
		{
			// Print if the terminal changed:
			engine.output.Print();
		}

		// Continue to the next tick after timing it.
		engine.time.WaitUntilNextTick();				// (10)
	}
	/*
		After existing the game loop, your game should prepare to quit (e.g. by saving player progress).
	*/

	/*
		Let's understand each step:

		1:	`Engine::running` is a simple `bool` variable that states whether the game loop should keep iterating, or exit. It's `true` by default, and can be set to `false` from anywhere to safely exit the game. For example, it is automatically set to `false` when the input engine component receives the quit key (Control+C by default). This leads to your game loop exiting, allowing you to prepare before the game actually quits when `main()` reaches its end (for example, by saving player progress).

		2:	`Engine::input` is the input engine component. `Input::CallCallbacks()` distributes player inputs to your functions, which we will try later.

		3:	`Engine::time` is the time engine component. `Time::CallInvocations()` calls functions that were invoked (by you directly or indirectly) to be called in this tick. We will also invoke functions in a later chapter.

		4:	`Engine::memory` is the memory engine component. `Memory::CallOnTicks()` calls the `OnTick()` function of all world structures. Of course, we will utilize this functionality as well, but later.

		5:	`Engine::output` is the output engine component. `Output::ShouldRenderThisTick()` answers the following question: "did anything change by calling those previous 3 functions?" If it returns `true`, it means we should render our game again because the previously printed frame is outdated.

		6: `Camera::Render()` renders all of the `Object`s of all of the `Layer`s in its parent `Map`.

		7: `Camera::Draw()` draws the just-rendered image to the output engine component's image buffer.

		8: `Output::Print()` prints the image buffer to the terminal, for the player to see.

		9: `Output::ShouldPrintThisTick()` answers the following question: "did the terminal change its size?" If it returns `true`, it means that the last image that was printed to the terminal got squashed or wasn't full, so we should print it again to display it correctly in the terminal.

		10:	`Time::WaitUntilNextTick()` enters sleep (temporarily set the thread to an inactive state) for the duration of time that will make our game loop run at a consistent rate, also know as the "ticks per second" (TPS) rate. It's the value we specified previously in `Engine`'s constructor, 24. That means this game loop will iterate 24 times every second, which includes updating the game's state, rendering it (assuming it changes every tick), and printing it, 24 times every second.
	}
	*/
}