# Preamble

This document contains answers for potential questions.

- [How to build KTech?](#how-to-build-ktech)
- [How does the licensing work (with Premake)?](#how-to-build-ktech-with-premake)
- [Is KTech Stable?](#is-ktech-stable)
- [Why are `Collider`'s and `Texture`'s constructors normal functions?](#why-are-colliders-and-textures-constructors-normal-functions)
- [Why is there no predefined game loop?](#why-is-there-no-predefined-game-loop)
- [How does the file system work?](#how-does-the-file-system-work)

## How to build KTech (with Premake)?

KTech uses **Premake** to configure and generate build files. CMake is not used because of the counterintuitive documentation and odd scripting language. Premake is used instead because of the [truly nice documentation](https://premake.github.io/docs/) and usage of the proper scripting language Lua, which is not even required learning directly to start writing Premake scripts. If you aren't familiar with Premake I recommend having a look at the "Getting Started" and "Writing Premake Scripts" sections of its documentation, which are short and well written. The following text assumes you've read those sections.

The `"ktech/"` directory contains a [`"premake5.lua"`](/ktech/premake5.lua) script, which creates a Premake static library project named "KTechLibrary".

To link the library, you will have to create a `"premake5.lua"` script for your own game that should do the following things:
- Create a Premake workspace.
- `include` the library's `"premake5.lua"` script (i.e. `include "path/to/ktech/"`).
- Create a `ConsoleApp` project for your game, and within it link "`KTechLibrary`" (i.e. `links { "KTechLibrary" }`).

For example (`--` is a Lua comment):

```lua
workspace "KTech" -- this name doesn't matter to KTech
	configurations { "Debug", "Release" }
	location "build" -- where building files will be generated

include "ktech/" -- this should lead to the KTech library directory

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

To generate the build files, you will need to call `premake5 [action]` from the directory your script is in (e.g. `premake5 gmake2` to generate GNU makefiles).

To generate binary files with GNU Make, you will then run `make -C workspace_build_dir`, where "workspace_build_dir" leads to the `location` you specified in your `workspace`. The binary file of your game will be generated where the `targetdir` you specified in your `project` leads.

## How does the licensing work?

KTech is licensed under [the GNU General Public License version 3 (GPLv3)](https://www.gnu.org/licenses/gpl-3.0.en.html) or any later version. Read the article ["A Quick Guide to GPLv3"](https://www.gnu.org/licenses/quick-guide-gplv3.html) from GNU's website to learn about it. You are completely allowed to sell copies of your game (for example, via Steam), and so does anyone else, though I am not sure how significantly this affects sales in practice.

Using the Steamworks API with KTech (to utilize Steam achievements, for instance) contradicts the GPLv3 due to it not falling under the definition of "System Libraries" (thus, not being exempted from the copyleft). As a solution, I might in the future decide to specifically allow using the Steamworks API with KTech.

## Is KTech Stable?

KTech is reasonably uncomplicated and is written under pretty rigid code conventions (see [style.md](style.md)). Despite so, I frequently encounter bugs. This is very much not to be unexpected, as KTech has seen much more coding done on it than actual usage. Parallel to the last time I've worked on my game _netset_, I've composed what would seem a worryingly long list of overlooked parts (see [issue #58](https://github.com/TheRealKaup/KTech/issues/58)). Solving most of them was as easy as finding them. The asnwer to the question: no. But this just means KTech is not fundamentally faulty, and rather it needs more testing and feedback.

## Why are `Collider`'s and `Texture`'s constructors normal functions?

Using normal constructors was ambiguous to the coder and the compiler in this case. The writing convention of `vector.resize(n)` and `vector[i].Constructor()`, where "vector" is a vector of `Texture`s or `Collider`s, is just the simple and easy alternative I found. I might change this method, maybe to a factory design.

## Why is there no predefined game loop?

It seems to me preferable forcing to write a game loop and understand how it works. Depends on how extensible a predefined game loop should be, it might require an amount of parameters and new features that is equivalent in code complexity to writing a game loop on the game side as is done now.

## How does the file system work?

KTech is provided as a static library. It's declared and defined within [the `"ktech/"` directory of this repository](https://github.com/TheRealKaup/KTech/tree/master/ktech).

The core library is declared in `"ktech/ktech.hpp"`. This file is what your game source files should `#include`.

`"ktech.hpp"` contains forward declarations, all of which are enclosed within a namespace named `KTech`. Classes are defined in the other header files (`".hpp"`), and class members are usually defined in additional source files (`".cpp"`).

The non-core parts of the library are so far only the optional predefined UI elements ("widgets") that are stored in `"ktech/widgets/"`. If you want to use any of KTech's widgets, you may selectively include the header files from that directory.

The contents of `"ktech/"` is split into multiple sub-directories:
- `"basic/"` - basic classes.
- `"engine/"` - the `Engine` class and engine components.
- `"utility/"` - utility classes.
- `"widgets/"` - optional widgets that you may include from here.
- `"world/"` - world classes.

Note that `"ktech.hpp"` includes all of required header files only if `KTECH_DEFINITION` is undefined. This macro is an include guard completely internal to KTech, and you shouldn't define it before including `"ktech.hpp"`. Apart from that, don't worry about this macro.