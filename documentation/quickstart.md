# Preamble

This document contains a short tutorial on using KTech. It does not cover all of the library.

If you are looking for the library reference, refer to the [documentation section in the `"readme.md"` file](https://github.com/TheRealKaup/KTech/blob/master/readme.md#documentation) (it isn't available yet, and right now this tutorial is all there is).

- [Key Concepts](#key-concepts)
	- [Licensing](#licensing)
	- [The File System](#the-file-system)
	- [World Hierarchy](#world-hierarchy)
	- [Engine](#engine)
	- [Game Loop](#game-loop)
- [Creating a Game](#creating-a-game)
	- [Project Setup](#project-setup)
	- [Game Example](#game-example)
	- [Building (Premake)](#building-premake)

# Key Concepts

## Licensing

KTech is licensed under [the GNU General Public License version 3 (GPLv3)](https://www.gnu.org/licenses/gpl-3.0.en.html) or any later version. Read the article ["A Quick Guide to GPLv3"](https://www.gnu.org/licenses/quick-guide-gplv3.html) from GNU's website to learn about it. You are completely allowed to sell copies of your game (for example, via Steam), and so does anyone else, though I am not sure how significantly this affects sales in practice.

Using the Steamworks API with KTech (to utilize Steam achievements, for instance) contradicts the GPLv3 due to it not falling under the definition of "System Libraries" (thus, not being exempted from the copyleft). As a solution, I might in the future decide to specifically allow using the Steamworks API with KTech.

## The File System

KTech is provided as a static library. It's in [the `"ktech/"` directory in this repository](https://github.com/TheRealKaup/KTech/tree/master/ktech).

The core library is declared in the `"ktech/ktech.hpp"`. This file is what you `#include` in your game source files.

The content of `"ktech.hpp"` are enclosed within a namespace called `KTech`. This file only include class forward declarations. Class definitions are stored in the rest of the header files (`".hpp"`) in `"ktech/"`, and the definitions of class members are usually stored in additional source files (`".cpp"`).

The non-core parts of the library are so far only the optional UI elements ("widgets") that are stored in the `"ktech/widgets/"` directory. If you want to use any of the widgets there, you may selectively include the header files in your game source files.

Notice that `"ktech.hpp"` includes all of the header files that have class definitions only if `KTECH_DEFINITION` hasn't been defined. This macro behaves as an include guard within KTech, and you should not have it defined in your game source files before including `"ktech.hpp"` yourself, so don't worry about it.

The content of `"ktech/"` is split into directories:
- `"basic/"` - basic classes.
- `"engine/"` - `Engine` class and engine components.
- `"utility/"` - utility classes.
- `"widgets/"` - optional widgets that you may include from here.
- `"world/"` - world classes.

## Basic Structures

KTech uses some simple structures:
- `RGB` - Represents a single 24 bit depth color.
- `RGBA` - Represents a single 24 bit depth color with an alpha channel.
- `Point` - Comprises 2 32 bit signed integers (`x` and `y`), usually represents a 2D position.
- `UPoint` - Comprises 2 32 bit unsigned integers (`x` and `y`), usually represents a size.
- `Cell` - Represents a terminal cell, comprising a foreground `RGB`, background `RGB` and a `char`.
- `CellA` - Represents a terminal cell with alpha channels, comprising a foreground `RGBA`, background `RGBA` and a `char`.

## World Structures

The library contains more complex classes that end up building a game world. These structures are `Map`, `Layer`, `Camera`, `Object`, `Texture`, `Collider`, `UI` and `Widget`:

Hierarchy:

```
Map
|_____
|     \
Layer  Camera      UI
|                  |
Object             Widgets
|_______
|       \
Texture  Collider
```

In general:

### `Map`

Isolated collection of `Layer`s and `Camera`s (`Map`s do not interact with each other). They are a good way to divide your game to levels or dimensions, for instance.

### `Layer`

Collection of `Object`s. Rendering multiple `Layer`s means that the `Texture`s from the first `Layer` will be shown below `Texture`s from the second or following `Layer`s. Collision-wise, `Object`s interact with other `Object`s from the same `Layer`.  Contains `Object`s.

### `Object`

Comprise a collection of `Texture`s and `Collider`s that behave relative to the position of their parent `Object`.

### `Texture`

Represents the 2D appearance of `Object`s. There are simple and complex `Texture`s. Complex `Texture`s store an array of different `CellA` values, while simple `Texture`s store a singular `CellA` which makes them a plain rectangle. `Complex` textures are more visually capable, but `Simple` textures are significantly faster to render and need less memory.

### `Collider`

Represents the 2D physical space of `Object`s. In a similar fashion to `Textures`, there are simple and complex `Collider`s. Both simple and complex `Collider`s are based on `bool` values, and have a singular "type", which is used to determine their collision result with other colliders (block, push, or overlap).

### `Camera`

Able to render `Layer`s containing `Object`s with `Texture`s into a `Cell`-based image that can be given to the engine in order to be printed to the terminal.

### `UI`

Behaves both as a `Layer` and a `Camera`, for `Widget`s.

### `Widget`

A user interface element. Similar to `Object` but doesn't have `Colliders`. Provides basic state-machine features (selecting, deselecting, showing, hiding and nesting).

## `Engine`

The `Engine` class is built of engine components that work together in order to make your game run: `Collision`, `Input`, `Memory`, `Output` and `Time`.

In general:

### `Collision`

Moves `Object`s on demand. It does that by processing how given `Object` movement would look like, updates positions of affected `Object`s and calls the relevant function callbacks.

**Editor note: the following `Collision` explanation should be moved to a section that is intentionally thorough.**

These said callbacks are virtual `Object` functions (such as `Object::OnBlocked()` and `Object::OnOverlap()`) that you may override by making a derived class of `Object`.

`Collision` also stores `colliderTypes`, which is a 2D vector that determines collision results between `Object`s. You can look at `colliderTypes` this way:

| What if (row) collides with (column) | Type 1  | Type 2  | Type 3  |
|--------------------------------------|---------|---------|---------|
| Type 1                               | Block   | Push    | Overlap |
| Type 2                               | Block   | Push    | Overlap |
| Type 3                               | Overlap | Overlap | Overlap |

Meaning, type 1 is a collider that can't be pushed, type 2 can be pushed, and type 3 overlaps with everything.

### `Input`

Calls registered callback functions when a given key is pressed. Also offers a way to group callback functions to ease distributing key presses in a UI.

### `Memory`

Holds pointers to all created world structures, and makes accessing them serializable with `ID`s.

### `Output`

Draws a "final image" using images rendered by `Camera` and `UI`. The final image can then be printed to the terminal. This extra drawing step allows for multiple rendered images to be shown at the same time.

### `Time`

Provides game loop utilities. It can also create future invocations given specified time.

## Game Loop

The game loop is what coordinates the timing of game ticks, and with KTech, the use needs to create their own.

A tick is a singular game iteration, which usually does the following tasks in the following order:

- Call callback functions:
	- Distribute input (`Input::CallHandlers()`)
	- Due invocations (`Time::CallInvocations()`)
	- On-tick functions (`Memory::CallOnTicks()`)
- Output the current state of the game:
	- Render (`Camera::Render()`, `UI::Render()`)
	- Draw (`Output::Draw()`)
	- Print (`Output::Print()`)
- Continue to the next tick:
	- Enter the thread to sleep for a duration of how long should a tick be, subtracted by how long the current tick elapsed (`Time::WaitUntilNextTick()`)

The simplest form of a game loop follows these tasks each iteration, and exists when `Engine::running` turns false.

By letting the user create their own game loop, they can add and remove tasks, and arrange them however they want.

It is recommended to also implement a "render-on-demand" mechanism using `Output::ShouldRenderThisTick()` and `Output::ShouldPrintThisTick()`, to significantly improve resource usage.

# Creating a Game

## Project Setup

- Create a source file for your game (for example, `"game.cpp"`).
- Create a copy of the KTech library ([the `"ktech/"` directory in this repository](https://github.com/TheRealKaup/KTech/tree/master/ktech)), in a place accessible from you game source file (for example, in the same directory).
- `#include` the `"ktech.hpp"` header file from the library in your game source file.

You should end up with a game root directory that looks like this:

- `"game.cpp"`: `#include "ktech/ktech.hpp"`
- `"ktech/"`
  - `ktech.hpp`
  - the rest of the library...

Create an additional file named `premake5.lua` in your root directory, and follow the [Building](#building) section to script it.

## Game Example

[`"examples/quickstart/game.cpp"`](https://github.com/TheRealKaup/KTech/tree/master/examples/quickstart/game.cpp) contains a very simple and well-commented KTech game example. It shows how to create a moving character, initialize a game, and construct a game loop.

## Building (Premake)

KTech uses **Premake** to configure and generate build files. CMake is not used because of the counterintuitive documentation and odd scripting language. Premake is used instead because of the [truly nice documentation](https://premake.github.io/docs/) and usage of the proper scripting language Lua, which is not even required learning directly to start writing Premake scripts. If you aren't familiar with Premake I recommend having a look at the "Getting Started" and "Writing Premake Scripts" sections of its documentation, which are short and well written. The following text assumes you've read those sections.

The `"ktech/"` directory contains a [`"premake5.lua"`](/ktech/premake5.lua) script, which creates a Premake project named "KTechLibrary" for the KTech static library.

To link the library will have to create a `"premake5.lua"` script for your game that should do the following things:
- Create a Premake workspace.
- `include` the library's `"premake5.lua"` script (i.e. `include "path/to/ktech/dir"`.
- Create a `ConsoleApp` project, and link "`KTechLibrary`".

To generate the build files, you will need to call `premake5` from the directory with the script, with the following platform argument. If you are on Gnu/Linux, you would call `premake5 gmake2`, and then to build the library and your game, you will use `make`.

```lua
workspace "KTech" -- this name doesn't actually matter to the KTech library itself
	configurations { "Debug", "Release" }
	location "build" -- where building files will be generated

include "ktech" -- this should lead to the KTech library directory

project "ktechgame" -- the name of your game
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin" -- where binary files will be generated
	objdir "%{wks.location}/obj/%{prj.name}" -- where object files will be generated
	targetname "%{prj.name}" -- the name of the binary file

	links { "KTechLibrary" } -- include the KTech static library

	files { "*.cpp", "*.hpp" } -- include your game source files

	filter "configurations:Debug"
		symbols "On" -- turn on symbols if you want to debug
```

On Gnu/Linux you will run `$ premake5 gmake2` from where your `"premake5.lua"` is located, which will generate the build configuration files. With `make` you will then run `$ make -C workspace_build_dir`, where "workspace_build_dir" is the `location` you specified in the `workspace` settings.

The binary file will be generated `workspace:location/project:targetdir`, as you have specified in your `"premake5.lua"` file.