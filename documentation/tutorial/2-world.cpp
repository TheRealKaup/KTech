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
		Before creating any game objects, rendering them and such, we need a `KTech::Engine` class instance.

		The `Engine` class contains everything needed to manage a game and its world structures: input, output, collision, memory and time "engine components". These engine components are all initialized in `Engine`, and are accessible through your `Engine` instance. They hold many useful functions and variables (as well as do background tasks), so being aware of the different engine components is helpful to effectively find what you need.
	*/

	// Create an `Engine` instance:
	KTech::Engine engine(
		KTech::UPoint(20, 20), // The game's viewport size, in terminal cells.
		24 // The ticks-per-second (TPS) limit; we'll cover this soon.
	);
	// You might find it convenient to instantiate `Engine` outside `main()`, especially when working on a larger-scale game. Feel free to do so; it doesn't matter to `Engine`. Beware, though, not to unintentionally create multiple `Engine` instances, as doing so would make some of the duplicate engine components fight each other and cause random and unexpected behavior.

	/*
		Now that we have an `Engine` instance, we can create world structures.

		"World structures" are best introduced with their hierarchy:

			Map              UI            `KTech::Map` stores `KTech::Layer`s and `KTech::Camera`s.
			|_______         |             `KTech::Layer` stores `KTech::Object`s.
			v      v         v             `KTech::UI` stores `KTech::Widget`s.
			Layer  Camera    Widget
			|
			v
			Object
	*/

	// Let's start with the easiest one to explain: `Object`.
	// All world structures' constructors require a reference to an `Engine` instance, which is why you have to create an `Engine` first. `Object` is no exception:
	KTech::Object object1(
		engine,				// Parent `Engine` reference.
		KTech::Point(5, 5) 	// Position the `Object` at X=5, Y=5.
	);

	/*
		World structures need this `Engine` reference to register themselves at its memory component, and access it later to communicate with other world structures. The memory engine component assigns world structures a serializable `KTech::ID` so they can store, access and call each other. You need not worry now about how `ID`s work, but know that they are how world structures are usually referenced, and that they are the reason the first parameter of all world structure constructors is a required `Engine` reference.

		If you are wondering: early on in KTech's past, world structures would communicate with each other via memory addresses (pointers). It was worse in terms of memory safety, and it wasn't serializable (which in turn means incompatible with multiplayer).
	*/

	/*
		Our `Object` is empty, so let's give it `Texture`s.

		`Texture`s are 2D terminal bitmaps. Because they are terminal-apt, instead of the usual pixels you are familiar with, `Texture`s are composed of terminal cells, specifically, the `KTech::CellA` structure, which comprises a character, foreground (character) color and background color.

		`Object`s store their `Texture`s as instances within a vector of `Texture`s by the name of `Object::m_textures`. First we need to create blank `Texture`s by setting the vector to a size, and then draw on them.
	*/

	// Currently we plan `object1` to have a single `Texture`, so we'll size `m_textures` to 1.
	object1.m_textures.resize(1);

	// Now access `Object:m_textures[0]` (that is, its first `Texture`), and make it a simple rectangle:
	object1.m_textures[0].Simple(			// (1)
		KTech::UPoint(3, 3),				// (2)
		KTech::CellA(						// (3)
			'@',							// (4)
			KTech::RGBA(255, 0, 255, 255),	// (5)
			KTech::RGBAColors::blue			// (6)
		),
		KTech::Point(0, 0)					// (7)
	);

	/*
		Let's look at the previous function's parameters in greater detail:

		1. `Texture::Simple()` is one of `Texture` design functions. We'll explore all of them later in the tutorial.

		2. 3 by 3 square, but it's worth noting that terminal cells (depending on the terminal's font) usually aren't square themselves (but more like narrow rectangles), so it's likely that this `Texture` won't end up being an exact square when printed.

		3. As we've said, this "`CellA`" is the structure `Texture`s are based on. `Texture::Simple()` accepts a single `CellA` value, because it creates a uniform rectangle.

		4. Character.

		5. Pink foreground color (the character's color).

		6. Blue background color (the cell's color behind the character). The `KTech::RGBAColors` namespace includes some predefined `RGBA` values.

		7. Position, relative to the `Texture`'s parent `Object`. Extremely useful when adding multiple `Texture`s to an `Object`.

		Don't forget the Doxygen API reference, which includes information on all function parameters and variables in KTech.
	*/

	/*
		We've already came across a lot of KTech's "basic structures" (like `Point` and `CellA`). Let's quickly examine them all before proceeding:

		- `KTech::Point` - 2D vector, mostly used to store positions and directions.
		- `KTech::UPoint` - Unsigned 2D vector, mostly used to store sizes and 2D indexes.
		- `KTech::RGB` - 24-bit reg-green-blue color, able of representing 16,777,216 (2^24) different colors.
		- `KTech::RGBA` - Like `RGB`, but also has an alpha channel representing transparency.
		- `KTech::Cell` - Terminal cell comprising `RGB` foreground (character) color, `RGB` background color, and an ASCII character.
		- `KTech::CellA` - Like `Cell`, but with `RGBA` foreground and background colors, instead of `RGB`.
	*/

	/*
		An `Object` needs to be placed in a `KTech::Layer`, otherwise it's located nowhere and deemed inaccessible, meaning, other `Object`s can't interact with it and `Camera`s (shown later in this chapter) can't render it.

		A `Layer` contains `Object`s so they can physically interact with each other by means of collision (a topic for a later chapter).
	*/

	// Create a `Layer` (it only needs an `Engine` reference):
	KTech::Layer layer1(engine);

	// And add `object1` to it:
	layer1.AddObject(
		object1.m_id // All world structures store their personal `ID` in a member named `m_id` (e.g. `Object::m_id`, `Layer::m_id`, `Camera::m_id`...).
	);

	/*
		Adding a child world structure, to a parent world structure, can be done in multiple ways:

		1. From the child's constructor (if the parent was created first), e.g.:

			`KTech::Object object1(engine, layer1.m_id);`

		2. Using the parent's "adder" function, e.g.:

			`layer1.AddObject(object1.m_id);`

		3. Using the child's "enterer" function, e.g.:

			`object1.EnterLayer(layer1.m_id);`

		All methods result the same, so the choice of which to use should be based on your personal preference.
	*/

	/*
		`Layer`s and `Camera`s should be added to a `KTech::Map`. The option to have multiple `Layer`s with `Object`s in a `Map` means two major things:

		1. You can control the order `Object`s in a `Map` are rendered: `Layer`s are rendered in the order they were added to the `Map`, meaning, `Object`s in a `Layer` added later will appear on top of `Object`s in a `Layer` added earlier.

		2. You can control which `Object`s can collide with each other by splitting them into `Layer`s.

		In a sense, `Layer`s are like physical "levels" or "floors" stacked on top of each other on the Z axis; `Object`s move on the X and Y axes so they can collide only with other `Object`s from the same Z level (i.e. `Layer`), while `Camera`s view the `Map` from the top of the Z axis so they can see all `Object`s from all `Layer`s.
	*/

	// Create a `Map`:
	KTech::Map map1(engine);

	// Create a `Camera`:
	KTech::Camera camera1(
		engine,
		map1.m_id, 					// Add `camera1` to `map1`.
		KTech::Point(0, 0),			// Position the `Camera` at X=0, Y=0 (world's origin).
		engine.output.resolution	// `Engine::output` is the output engine component instance, and `Output::resolution` is the same viewport size we gave to `Engine`'s constructor earlier.
	);

	// Add `layer1` to `map1`:
	layer1.EnterMap(map1.m_id);

	/*
		We conclude with the following world hierarchy:

			map1
			|________
			v       v
			layer1  camera1
			|
			v
			object1

		Build, and then run this program; nothing will happen. That's because we're missing the game loop which we'll write in the next chapter.
	*/
}