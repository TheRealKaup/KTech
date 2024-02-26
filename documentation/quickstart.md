# Table of Contents:

- [Table of Contents](#table-of-contents)
- [Introduction to Quick Start Guide](#introduction-to-quick-start-guide)
- [Key Concepts](#key-concepts)
  - [The File System](#the-file-system)
  - [World Hierarchy](#world-hierarchy)
  - [Engine](#engine)
  - [Game Loop](#game-loop)
  - [Widgets](#widgets)
- [Game Example](#game-example)
- [Building](#building)
  
# Introduction to Quick Start Guide

This document is a short tutorial on using KTech. Note that it covers only a small portion of the library and minimally explains how to use it.

If you are looking for a concise tutorial, or the library reference, refer to the [documentation section in the `readme.md` file](https://github.com/TheRealKaup/KTech/blob/master/readme.md#documentation) (both aren't available yet, and as for now this tutorial is KTech's entire documentation).

# Key Concepts

## The File System

KTech is a static library. It's in [the `ktech` directory in this repository](https://github.com/TheRealKaup/KTech/tree/master/ktech).

The core library is declared in the `ktech.hpp` header file of the `ktech` directory. This file is what you include in your game project. This is the "core" library since it is the minimum required to include in your project to start creating a game.

The contents of `ktech.hpp` are enclosed within a namespace called `KTech`.

The source files (`.cpp`) in the `ktech` directory are the core library's definitions.

The non-core parts of the library are currently only the widgets (UI objects) that are in the `ktech/widgets` directory. If you want to use any of the widgets there, you may selectively include the header files in there.

## Basic Structures

KTech comes with a couple of fundamental structures that are available via the `KTech` namespace. They are:
- `RGB` - Represents a single 24 bit depth color.
- `RGBA` - Represents a single 24 bit depth color with an extra alpha channel.
- `Point` - 2D vector comprising 2 32 bit signed integers (`x` and `y`).
- `UPoint` - 2D vector comprising 2 32 bit unsigned integers (`x` and `y`).
- `Cell` - Represents a terminal cell comprising of a foreground `RGB`, background `RGB` and a `char`.
- `CellA` - Represents a terminal cell with alpha channels comprising of a foreground `RGBA`, background `RGBA` and a `char`.

## World Hierarchy

The library contains more complex structures that build up a world. These structures are `Map`, `Layer`, `Camera`, `Object`, `Texture` and `Collider`, which are available via the `KTech` namespace.

```
          Map
         /   \
      Layer  Camera
       /      
    Object  
     /   \
Texture  Collider
```

### `Map`

Maps are completely disconnected from each other. They are a good way to divide your game to levels, for example. Maps contain a list of their layers.

### `Layer`

Layers are desigend to be loaded together. Layers contain a list of their objects.

When you render a map, it will render all of the layers in it, in the order they where added to the map. Meaning, textures in the first layer will be rendered first, and textures in the second layer will be rendered on top.

Physics-wise, objects interact only with other objects in the same layer.

### `Object`

Objects comprise textures and collider that behave relative to their parent object's position. Textures represent the object's appearance, and colliders represent the object's physical space.

### `Texture`

There are simple and complex textures, that both comprise of `CellA` values, beacuse textures are meant to be rendered for the terminal. Complex colliders are 2D arrays. Simple textures are instead rectangles, and thus are faster to render and have smaller memory usage, but they are of course less capable than complex colliders.

### `Colliders`

There are simple and complex colliders, in a similar manner to textures. Complex colliders are made of 2D array of `bool`s to represent where there is collider and where there isn't. Both complex and simple colliders have a `type` value, which is used to determine their collision result with other colliders.

Collision between objects can result in a push, a block, or an overlap event.

### `Camera`

Cameras are able to render a `Cell` based image that can be printed to the terminal, given layers that contain objects with textures.

## `Engine`

The `Engine` class comprises components that make the game run. These components manage different aspects, and are:
- `Collision` - Manages movement of objects and calculates collision events.
- `Audio` - Manages audio sources and playing audio
- `Time` - Manages invocations and provides game loop utilities
- `IO` - Manages user input and printing graphics
- `Memory` - Manages linking the world structure together

### `Collision`

Gets requests to move a certain object, calculates how its movement would look like, and based on that updates positions of objects and calls the relevant collision events on the relevant objects. Object movement and `Collision` work on demand and immediately return results and update the positions of objects after being called.

If you would look inside `ktech.hpp` at `Object` you would see a lot of virtual functions. These are the callback functions that `Collision` could call when it determines collision events.

You can create a class derived of `Object` and define new functionalities for these virtual functions to make them do things.

`Collision` also stores `colliderTypes` which represents the types of colliders that is global for the `Engine` instance. You can look at `colliderTypes` this way:

| What if (column) collides with (row) | Collider type 1 | Collider type 2 | Collider type 3 |
|--------------------------------------|-----------------|-----------------|-----------------|
| Collider type 1                      | Block           | Push            | Overlap         |
| Collider type 2                      | Block           | Push            | Overlap         |
| Collider type 3                      | Overlap         | Overlap         | Overlap         |

Meaning, type 1 is an unpushable collider, type 2 is a pushable collider, and type 3 is a collider that overlaps with everything.

### `Audio`

Currently `Audio` has no connections to the other components of the engine, and pretty much works as a wrapper to the "PortAudio" library.

`Audio` can load .wav files and create 1D audio sources, that can play, pause, resume and stop playing. It is pretty primitive.

### `Time`

`Time` provides utilities used to create a game loop (which will be discussed soon).

It can also create invocations that call given callback functions.

### `IO`

`IO` manages both input and output, although they are quite separate.

As for input, `IO` can be asked to attach a callback function when a certain key is pressed. It can also make groups of these attached callback functions, which make input handling easier, especially when it comes to UI.

As for output, `IO` can receive images rendered by `Camera`s to draw a "final image" that can later be actually printed to the terminal. The reason there is an extra step of drawing between rendering and printing is to allow multiple cameras at the same time to be displayed which can be useful for whatever reason.

### `Memory`

`Memory` is the serialization solution for the world hierarchy. It contains a `Container` for each of the storable structures - `Map`, `Layer`, `Camera` and `Object`.

The containers receive pointers to structures and return an `ID` that can be used to reach the structure again.

## Game Loop

The game loop in this context is a loop that attempts to run at a consistent speed. Iterations are called game ticks. Each tick usually does the same tasks, such as processing user input (which causes other events such as object movements), rendering, drawing and printing.

The `Engine` class nor any other part of the library provides a premade game loop function. Instead, the user constructs one with the functions provided in the `Time` component of `Engine`.

## `Widget`s

Widgets are UI elements derived from the `Widget` class which is derived from the `Object` class.

In the KTech ecosystem they are unrequired additions that you can include in your game as you wish.

They collect user input automatically and can be enabled and disabled with their universal `Select()` and `Deselect()` functions.

The currently available widgets are:
- Button
- Integer input field
- String input field
- Switch

# Creating a Game

## Setting Up a Project

- Create a source file for your game (such as `game.cpp`).
- Create a copy of the KTech library, that is, [the `ktech` directory in this repository](https://github.com/TheRealKaup/KTech/tree/master/ktech), in a place accessible to your game source file (e.g. in the same directory).
- Include the `ktech.hpp` header file from the library in your game soruce file.

## Game Example

### Engine

To start creating world structures you must first create an `Engine` instance:

```c++
#include "ktech/ktech.hpp"

static KTech::Engine engine(KTech::UPoint(40, 20), 24);
```

Here we are creating a file `Engine` instance.

The first `UPoint` argument is the size of the viewport, which is set manually rather than automatically with the size of the terminal itself.

The integer argument is the ticks per second (tps) limit, that is, the speed of the game loop. Normally this will also be the graphical frames per second (fps) limit. That can change at any point while the game is running.

### Creating a World

To create world structures and connect them together, you can create them on the stack or on the heap, but you have to pass a reference to an `Engine` so the structures could communicate with each other in a memory safe and serializable way.

For example, here we are creating a `Map` with a `Camera` and a `Layer` that has an `Object`:

```c++
int main()
{
	KTech::Map map(engine);

	KTech::Camera camera(engine, KTech::Point(0, 0), KTech::UPoint(40, 20));
	map.AddCamera(camera.id);

	KTech::Layer layer(engine, map.id);

	KTech::Object object(engine, KTech::Point(17, 7));
	object.EnterLayer(layer.id);
}
```

### Adding `Texture`s and `Collider`s to an `Object`

The `Object` from the previous code is empty, that is, it has no textures and no colliders.

`Object`s have 2 `std::vector`s, one with `Texture`s (`Object::textures`) and one with `Collider`s (`Object:colliders`). Normally, to add textures and colliders to an object you would resize those vectors to fit the amount of textures and colliders you want, and then use the functions within `Texture`s and `Collider`s to make something out of them.

For example, here we are adding to the object from before some textures and colliders:

```c++
	object.textures.resize(2); // Add textures to the object
	object.textures[0].Write(
		{
			" @@@ ",
			"@@@@@",
			"@@@@@",
		}, KTech::RGBA(0, 100, 0), KTech::RGBAColors::transparent, KTech::Point(0, -1)
	);
	object.textures[1].Simple(KTech::UPoint(1, 2), KTech::CellA('|', KTech::RGBA(80, 40, 15)), KTech::Point(2, 2));
	object.colliders.resize(2); // Add colliders to textures
	object.colliders[0].ByTextureCharacter(object.textures[0], 1, 0);
	object.colliders[1].Simple(KTech::UPoint(1, 2), 0, KTech::Point(2, 3));
```

And also add a background color to the layer so there's a sky:

```c++
	layer.brgba = KTech::RGBA(100, 200, 255, 255);
```

By the way, we are using the default `colliderTypes` of the `Collision` engine component.

### Game Loop

So far we have an engine and a world but nothing to run it. The solution is a game loop started after initializing the game:

```c++
	// Game loop
	while (engine.running)
	{
		// Start
		engine.time.StartThisTick();

		// Calls
		engine.io.Call();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		// Render, draw and print
		camera.Render(map.layers);
		engine.io.Draw(camera.image);
		engine.io.Print();

		// End
		engine.time.WaitUntilNextTick();
	}
```

This is the recommended general form of a KTech game loop, but you can of course change the order of the operations however you would like, as this the entire point of having to construct the game loop yourself. This is what each operation does:
- `engine.time.StartThisTick()` updates the engine component `Time`'s information about the time of which the tick started.
- `engine.io.Call()` calls callback functions that are registered to input keys that were pressed since the last call of this function.
- `engine.memory.CallOnTicks()` calls the `OnTick()` function present in the world structures contained in the memory containers.
- `camera.Render(map.layers)` makes the camera render the given layers, which in this case is the single layer added to the map earlier.
- `engine.io.Draw(camera.image)` takes the camera's rendered image and draws it to the engine's final image.
- `engine.io.Print()` finally prints the image to the terminal.
- `engine.time.WaitUntilNextTick()` will block the thread until the next tick should start.

### Player Character

You can inherit a class from `Object` and add functionality to it. This way you can create a user-controlled character.

For example:

```c++
struct Character : KTech::Object
{
	void Down()
	{
		Move(KTech::Point(0, 1));
	}
	void Up()
	{
		Move(KTech::Point(0, -1));
	}
	void Right()
	{
		Move(KTech::Point(1, 0));
	}
	void Left()
	{
		Move(KTech::Point(-1, 0));
	}

	Character(KTech::Engine& engine, KTech::ID<KTech::Layer>& layer, KTech::Point pos)
		: Object(engine, layer, pos, "character")
	{
		textures.resize(1);
		textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 200, 0, 0, 255 }, { 0, 0, 0, 0 }, { 0, 0 }
		);
		colliders.resize(2);
		colliders[0].ByTextureCharacter(textures[0], 100, 1);

		engine.io.RegisterCallback(KTech::Keys::down, std::bind(&Character::Down, this), true);
		engine.io.RegisterCallback(KTech::Keys::up, std::bind(&Character::Up, this), true);
		engine.io.RegisterCallback(KTech::Keys::right, std::bind(&Character::Right, this), true);
		engine.io.RegisterCallback(KTech::Keys::left, std::bind(&Character::Left, this), true);

		EnterLayer(layer);
	}
};
```

This `Object`-inherited class initializes by inheriting `Object`'s constructor and then adding textures and colliders.

I encourage you to see what `Object`'s constructor does for youself by reading its definition in `ktech/object.cpp`, but in summary, it memorizes the engine reference, sets a position and enters a layer. Normally you would use this constructor (or the other one which doesn't enter a layer) when inheriting from `Object` as shown in the example above.

After passing the given arguments to `Object`'s constructor, the `Character` constructor adds to itself a texture and a collider, and then it registered input callbacks.

`IO::RegisterCallback()` requires 3 arguments:
- `const std::string& stringKey` - The trigger key, which should be the string that `IO` receives from the terminal when the key is pressed. Simple character keys like letters and numbers are recived as `"a"` or `"2"`, and other keys are received as escape codes which are listed in the `KTech::Keys` namespace. Note that capitalization matters, and `IO` does not automatically translate, for example, `a` to `A`, and vice versa. You would need to register a callback for each capitalization if you want to get calls for both.
- `const std::function<void()>& callback` - Your callback function. To make it call a memeber function, `std::bind` is used to attach this character's pointer to it.
- `bool onTick` - If true, will memorize key presses until `IO::Call()` is called, which will finally call the callback function. If false, will immediately call your callback function when the key is received. Using the method of calling `IO::Call()` in your game loop each iteration, as done in the game loop example from earlier, is much more preferable when it comes to moving things, and could prevent physics glitches. On the other hand, not having you callback functions get called on tick is preferable when it comes to UI, as in, using widgets.

The input callbacks registered are for moving down, up, right and left. They do so by using the `Object::Move()` function which receives a `Point()` representing the relative targeted position that `Collision` will atempt to move the object to.

As you might have noticed, the down direction is positive Y, up is negative Y, left is negative X, and right is positive X. This is because the axis origin is at the top left corner of the terminal, since this is the point printing is started at.

To create a `Character` instance, you can, for example, add in your `main()` function before the game loop at the game initialization stage the following instantiation:
```C++
Character character(engine, layer.id, KTech::Point(2, 2));
```

# Building

KTech uses **Premake** to generate build files. CMake is not used because of the counterintuitive documentation and odd scripting language. Premake is used instead because of the [truly nice documentation](https://premake.github.io/docs/) and usage of the proper scripting language Lua, which is not even required dedicatedly learning to start writing Premake scripts. If you aren't familiar with Premake I recommend having a look at the "Getting Started" and "Writing Premake Scripts" sections of its documentation, they are short and well written. The following text assumes you've read those sections.

The `ktech` directory contains a [`premake5.lua`](/ktech/premake5.lua) script. You should read the file, but in summary, it does 2 things:
- Links portaudio for the workspace scope, making the following created projects in the workspace inherit that value and link portaudio. This is requried.
- Create a Premake project named "KTechLibrary" for the KTech static library.

To include the library in your game source files you will need to create a `premake5.lua` script that should do the following things:
- Create a Premake workspace (the library Premake project does not depend on any workspace settings, such as location or configurations).
- `include` the library's `premake5.lua` script (i.e. `include "<ktech_library_path>"`, e.g. `include "ktech"` from the repository's root).
- Create a project for your game:
  - Its `kind` should be set to `ConsoleApp`.
  - Its `language` should be set to `C++`.
  - It should link `KTechLibrary`.
  - It should add your game files, and probably a `targetdir`, `objdir` and `targetname`.

To generate the build files, you will need to call `premake5` from the directory with the script, with the following platform argument. If you are on Gnu/Linux, you would call `premake5 gmake2`, and then to build the library and your game, you will use `make`.

## Example

The `ktech/` directory is placed in the root of your game development directory ("`game_root/`"). It includes its `premake5.lua` script as it should.

`game_root/` also contains the game files - `ui.hpp`, `ui.cpp` and `main.cpp`.

Your `premake5.lua` script is located in `game_root/` and contains:

```Lua
-- Create a workspace that its projects will be built in `build/`
workspace "KTech"
	configurations { "Release" }
	location "build"

-- Include the KTech library
include "ktech"

-- Create a project/target for your game
project "MyKTechGame"
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin" -- The final executable binary file will be located in `game_root/build/bin/`.
	objdir "%{wks.location}/obj/%{prj.name}" -- The object files will be located in `game_root/build/obj/MyKTechGame/`.
	targetname "%{prj.name}" -- The final executable binary file will be named "MyKTechProject".

	links { "KTechLibrary" }
	
	files { "*.cpp", "*.hpp"  } -- Include all the files in this diretory that include these patterns, which will include "ui.hpp", "ui.cpp" and "main.cpp". 
```

On Gnu/Linux you will run from `game_root/` the command `premake5 gmake2`, which will generate build files in `game_root/build/`, and you will run `make -C build/` to build the `MyKTechGame` project.

The binary file will be outputted as `game_root/build/bin/MyKTechGame`, and can be run by calling `./build/bin/MyKTechGame` from `game_root/`.
