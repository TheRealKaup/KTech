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

/*
	So far we made a game loop that shows a static game, because the updating functions didn't have anything special to call. One such special thing to call can be your own "input callback function", which are called by the input engine component when a specified input is received.

	Let's make a moving character that has these input callback functions. The simplest way to do this is to start by basing a class on `KTech::Object`. `Object` is perfect for the job, because it has some useful functions like `Move()`, `OnTick()` and more which we'll test later. Inheriting directly from `Object` makes initializing our character class very nice and clear (in contrast to making a wrapper class that includes an `Object`, rather that inherit from it).
*/
class Character : KTech::Object
{
private:
	/*
		This peculiar `CallbacksGroup` class manages our input callback functions. We give it our functions, and through it we can easily manage them afterwards (something we will test further in another chapter). The only way to assign a function to an input in KTech is by doing so through a `CallbacksGroup` instance (even if your group will comprise only 1 callback function).

		This is necessary for memory safety, because `CallbacksGroup` binds the life cycle of our input callback functions to the lifetime of the `Object` to which the functions belong (i.e., it's a RAII wrapper). This avoid dangling pointers when our `Character` destructs, because then the `CallbacksGroup` also gets destructed, and in its destructor, it makes sure that `Character`'s member input callback functions won't be called by the input engine component anymore.
	*/
	KTech::Input::CallbacksGroup m_callbacksGroup;

public:
	/*
		Our `Character`'s constructor will take the usual parameters that `Object::Object()` (`Object`'s constructor) takes, and simply pass it to `Object::Object()`.

		Here we choose to also take and pass the optional parent layer parameter of `Object::Object()`. This comes in `ID<Layer>` form, which means "an `ID` of a `Layer` world structure". The fact that `ID` is a template makes it impossible for us to accidentally take and pass an `ID` of an unrelated world structure (like the `ID` of a `Camera`; an `Object` cannot go within a `Camera`).
	*/
	Character(KTech::Engine& engine, KTech::ID<KTech::Layer> parentLayer, KTech::Point position)
		: Object(engine, parentLayer, position),
		// `CallbacksGroup`'s constructor also wants an engine reference.
		// The second `bool` parameter states whether to activate its input callback functions right of the bat.
		m_callbacksGroup(engine, true)
	{
		// A single `Texture` will suffice.
		m_textures.resize(1);

		// `Texture::Write()` is another design function. It accepts a vector of strings, and converts them into a corresponding bitmap:
		m_textures[0].Write(
			{ // Vector of strings
				" o ",
				"/|\\",
				"/ \\"
			},
			KTech::RGBAColors::black,		// Foreground color.
			KTech::RGBAColors::transparent	// Transparent = no background.
		);

		/*
			`CallbacksGroup::RegisterCallback()` is the function that assigns our input callback function an actual input.

			The first parameter is an input value. You can find various input values in the `KTech::Keys` namespace. Inputs are received in what's called "ANSI escape sequences", and they might seem unusual. That's just how the input engine component gets player inputs from the terminal.

			The second parameter is a lambda function (the input callback function) that calls `Object::Move()` and returns its returns value. There's quite a bit to unpack here:

			-	`Object::Move()` returns `true` if the `Object` successfully moved, and `false` if another `Object` blocked it.

			-	The return value of input callbacks function signify that they changed something in the world (this ends up in `Output::ShouldRenderThisTick()`, telling the game loop that the frame is behind new world changes and it should be rendered again).

			-	 So, we return the value of `Object::Move()` back to the engine, which satisfies this return requirement.

			In this case, though, because `Object` has no `Collider`s and there are no other `Object`s in the parent `Layer`, our `Object` (i.e. `Character`) will always succeed to move. But still, you get the point.
		*/
		m_callbacksGroup.RegisterCallback(KTech::Keys::up, 		[this] { return Move(KTech::Point(0, -1)); });
		m_callbacksGroup.RegisterCallback(KTech::Keys::left, 	[this] { return Move(KTech::Point(-1, 0)); });
		m_callbacksGroup.RegisterCallback(KTech::Keys::down, 	[this] { return Move(KTech::Point(0, 1)); });
		m_callbacksGroup.RegisterCallback(KTech::Keys::right, 	[this] { return Move(KTech::Point(1, 0)); });
		/*
			`Object::Move()` asks for a "direction" parameter. Coordinates are in accordance with terminal printing (from left to right, from top to bottom). So:

			- X+ is right.
			- X- is left.
			- Y+ is down.
			- Y- is up.

			Here we register a callback function for each direction with its corresponding arrow key from the `KTech::Keys` namespace.
		*/
	}

	/*
		And that's it. There are many comments here, but overall, this is a pretty simple class. The only things we ourselves added to `Object` are:

		- `CallbacksGroup`.
		- Constructor that initializes `Object::m_textures`.
		- Constructor that registers movement input callback functions through the `CallbacksGroup`.

		We instantiate our `Character` in `main()`, like any other world structure. Don't forget to try to build and run this small example (`./build/bin/4-input`).
	*/
};

auto main() -> int
{
	// Engine
	KTech::Engine engine{KTech::UPoint(15, 8), 24};

	// Map
	KTech::Map map{engine};

	// Layer: add it to `map`.
	KTech::Layer layer{engine, map.m_id};

	// Simply creating our character instance should get things working.
	Character character(engine, layer.m_id, KTech::Point(5, 2));

	// Camera: add it to `Map`
	KTech::Camera camera{engine, map.m_id, KTech::Point(0, 0), engine.output.resolution};
	// Give the camera a background color (`Camera::m_background` is a `CellA`, and `CellA::b` is the `RGBA` background color).
	camera.m_background.b = KTech::RGBAColors::blue;

	// Enter game loop:
	while (engine.running)
	{
		// Update game
		engine.input.CallCallbacks();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		if (engine.output.ShouldRenderThisTick())
		{
			// Draw and print if the game changed:
			camera.Render();
			camera.Draw();
			engine.output.Print();
		}
		else if (engine.output.ShouldPrintThisTick())
		{
			// Print if the terminal changed:
			engine.output.Print();
		}

		engine.time.WaitUntilNextTick();
	}
}