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

int main ()
{
	/*
		Before initializing any game objects and such, we need a `KTech::Engine` class instance.
		
		The `Engine` class contains everything needed to manage a game and its world structures: input, output, collision, memory and time "engine components". Most of these engine components do their work completely on their own. They all initialize themselves when `Engine` is constructed and terminate when `Engine` is destructed, so you don't need to worry about them. Still, knowing about their existence comes in useful when looking for some engine function of variable you need, because virtually always, you are going to find it in one of the engine component classes.
	*/

	// Create an `Engine` instance:
	KTech::Engine engine(
		KTech::UPoint(20, 20), // The game's image (or "viewport") size in terminal cells, which should ideally fit within the terminal's viewport.
		24 // The ticks-per-second (TPS) limit; we'll cover this soon.
	);
	// You might find it convenient to instantiate `Engine` outside `main()`, especially when working on a larger-scale game. Feel free to do so; it doesn't matter to `Engine`. Beware, though, not to unintentionally create multiple `Engine` instances, as doing so would make some of the duplicate engine components fight each other and cause random and unexpected behavior. 

	/*
		Now that we have an `Engine` instance, we can create world structures.

		"World structures" are best introduced with their hierarchy:

			Map              UI            `KTech::Map` stores `KTech::Layer`s and `KTech::Camera`s.
			|_______         |             `KTech::Layer` stores `KTech::Object`s.
			v      v         v             `KTech::UI` stores `KTech::Widget`s. 
			Layer  Camera    Widget        `KTech::Widget` can store additional (nested) `KTech::Widget`s.
			|                | ^
			v                |_|
			Object
	*/

	// Let's start with the easiest one to explain: `Object`.
	// All world structures' constructors require a reference to an `Engine` instance, which is why you have to create an `Engine` first. `Object` is no exception:
	KTech::Object object1(
		engine, // `Engine` reference.
		KTech::Point(5, 5) // Position the `Object` at X=5, Y=5.
	);
	
	/*
		World structures need this `Engine` reference to register themselves at its memory component, and access it later to communicate with other world structures. The memory engine component assigns world structures a serializable `KTech::ID` structure so they can store, access and call each other. You need not worry now about how `ID`s work, but know that you are going to use them a lot, and that they are the reason the first parameter of all world structure constructors is a required `Engine` reference.

		If you are wondering: early on in KTech's past, world structures would communicate with each other via memory addresses (pointers). It was worse in terms of memory safety, and it wasn't serializable (which in turn means incompatible with multiplayer).



		Our `Object` is empty, so let's give it `Texture`s.
		
		`Texture`s are 2D terminal bitmaps. Because they are terminal-apt, instead of the usual pixels you are familiar with, `Texture`s are composed of terminal cells, specifically, the `KTech::CellA` structure, which comprises a character, foreground (character) color and background color.

		`Object`s store their `Texture`s as instances within a vector of `Texture`s by the name of `m_textures`. First we need to create blank `Texture`s by setting the vector to a size, and then draw on them.
	*/

	// Currently we plan `object1` to have a single `Texture`, so we'll size `m_textures` to 1.
	object1.m_textures.resize(1);
	// Now access `Object:m_textures[0]` (that is, the first `Texture`), and make it a simple rectangle:
	object1.m_textures[0].Simple( // `Texture::Simple()` is one of `Texture`'s pseudo-constructor functions. We'll explore all of them later in the tutorial.
		KTech::UPoint(3, 3), // 3 by 3 square, but it's worth noting that terminal cells (depending on the terminal's font) usually aren't square themselves (but more like narrow rectangles), so it's likely this `Texture` won't end up being an exact square when printed.
		KTech::CellA( // As we've said, this is the structure `Texture`s are based on. `Texture::Simple()` accepts a singular `CellA` value, because it creates a uniform rectangle.
			'@', // Character.
			KTech::RGBA(255, 0, 255, 255), // Pink foreground color (the character's color).
			KTech::RGBA(0, 255, 0, 255) // Blue background color (the cell's color behind the character).
		),
		KTech::Point(0, 0) // Position, relative to the `Texture`'s parent `Object`. Extremely useful when adding multiple `Texture`s to an `Object`. 
	);

	/*
		Practice: add another `Texture` to `object1`, using the `Texture::Simple()` pseudo-constructor. Don't forget to increase the size of `m_textures`.



		We've already came across a lot of KTech's "basic structures" (like `Point` and `CellA`). Let's quickly examine them all now before proceeding:
		
		- `KTech::Point` - 2D point made of signed integers. Usually represents a position.
		- `KTech::UPoint` - like `Point` but made of unsigned integers (instead of signed integers). Usually represents a size.
		- `KTech::RGB` - RGB (red, green, blue) color (24 bit depth).
		- `KTech::RGBA` - like `RGB`, but has an additional alpha channel for transparency effect.
		- `KTech::Cell` - terminal cell made of a `char`, a foreground `RGB` color (character's color), and a background `RGB` color (cell's color behind the character).
		- `KTech::CellA` - like `Cell` but made of a `RGBA`s (instead of `RGB`s).



		An `Object` needs to be placed in a `Layer`, otherwise it's located nowhere and deemed inaccessible, meaning other `Object`s can't interact with it and `Camera`s can't render it.

		A `Layer` contains `Object`s so they can physically interact with each other by means of collision (a topic for later).
	*/

	// Create a `Layer` (it only needs an `Engine` reference):
	KTech::Layer layer1(engine);
	// And add `object1` to it:
	layer1.AddObject(
		object1.m_id // All world structures store a copy of their personal `ID` in a member named `m_id` (e.g. `Object::m_id`, `Layer::m_id`...).
	);

	/*
		Adding a child world structure to a parent world structure can be done in multiple ways:
		
		- From the child's constructor (if the parent was created first), e.g.:
			`KTech::Object object1(engine, layer1.m_id);`
		- Using the parent's "adder" function, e.g.:
			`layer1.AddObject(object1.m_id);`
		- Using the child's "enterer" functions, e.g.:
			`object1.EnterLayer(layer1.m_id);`
		
		All methods result the same.



		`Layer`s and `Camera`s should be added to a `Map`. The option to have multiple `Layer`s with `Object`s in a `Map` means two major things:

		- You can control the order `Object`s in a `Map` are rendered: `Layer`s are rendered in the order they were added to the `Map`, meaning `Object`s in a `Layer` added later will appear on top of `Object`s in a `Layer` added earlier.
		- You can control which `Object`s can collide with each other by splitting them into `Layer`s. 

		In a sense, `Layer`s are like physical "levels" or "floors" stacked on top of each other on the Z axis; `Object`s move on the X and Y axes so they can collide only with other `Object`s from the same Z level (i.e. `Layer`), while `Camera`s view the `Map` from the top of the Z axis so they can see all `Object`s from all `Layer`s.
	*/

	// Create a `Map`:
	KTech::Map map1(engine);
	// Create a `Camera`:
	KTech::Camera camera1(
		engine,
		map1.m_id, // Add `camera1` to `map1`.
		true, // And do so as the "active" `Camera` in the `Map` (meaning the `Map` will use this `Camera` for rendering).
		KTech::Point(0, 0), // Position the `Camera` at X=0, Y=0 (world's origin).
		engine.output.resolution // `Engine::output` is the output engine component instance, and `Output::resolution` is the image size given to `Engine`'s constructor earlier.
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