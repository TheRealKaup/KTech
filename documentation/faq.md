# Preamble

This document contains answers for potential questions.

- [How to build KTech?](#how-to-build-ktech-with-premake)
- [How to build your own KTech game (with Premake)?](#how-to-build-your-own-ktech-game-with-premake)
- [How does the licensing work?](#how-does-the-licensing-work)
- [Is KTech Stable?](#is-ktech-stable)
- [Why are `Collider`'s and `Texture`'s constructors normal functions?](#why-are-colliders-and-textures-constructors-normal-functions)
- [Why is there no predefined game loop?](#why-is-there-no-predefined-game-loop)
- [How does the file system work?](#how-does-the-file-system-work)

## How to build KTech (with Premake)?

KTech uses **Premake** to configure and generate build files, because it has [fine documentation](https://premake.github.io/docs/) and is configured with the normal scripting language Lua, unlike CMake with its counterintuitive documentation and odd scripting language.

To generate the build files for KTech and the game examples in the "`examples/`" directory, ensure Premake is installed on your system and run the command `premake5 [action]` from the Git repository's root (e.g. `premake5 gmake2` to generate GNU Makefiles).

To then generate binary files with GNU Make, run the command `make -C build/`, and they will be outputted to "`build/bin/`".

## How to build your own KTech game (with Premake)?

If you aren't too familiar with Premake I recommend reading the "Getting Started" and "Writing Premake Scripts" sections of its [documentation](https://premake.github.io/docs/); they are short and well written.

The "`ktech/`" directory contains a ["`premake5.lua`"](/ktech/premake5.lua) script, which creates a static library Premake project named "`KTechLibrary`".

To link the library, you will have to create a "`premake5.lua`" script for your own game that should do the following things:
- Create a Premake workspace.
- Include the library's "`premake5.lua`" script.
- Create a `ConsoleApp` project for your game, and within it link `KTechLibrary`.

For example (`--` is a Lua comment):

```lua
workspace "KTech" -- this name doesn't matter to KTech
	configurations { "Debug", "Release" }
	location "build" -- where to generate build files (doesn't matter to KTech)

include "ktech/" -- KTech's directory, which contains its "`premake5.lua`" configuration file

project "ktechgame" -- the name of your game (doesn't matter to KTech)
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin" -- where to generate binary files
	objdir "%{wks.location}/obj/%{prj.name}" -- where to generate object files
	targetname "%{prj.name}" -- the name which will be given to your game's binary file

	links { "KTechLibrary" } -- include the KTech static library

	files { "*.cpp", "*.hpp" } -- include your game source files

	filter "configurations:Debug"
		symbols "On" -- turn on symbols if you want to debug
```

To generate the build files for KTech and your game, ensure Premake is installed on your system and run the command `premake5 [action]` from the directory your "`premake5.lua`" script is in (e.g. `premake5 gmake2` to generate GNU Makefiles).

To then generate binary files with GNU Make, run the command `make -C build/`, and they will be outputted to "`build/bin/`".

The "Debug" configuration (on KTech's end) enables debug symbols and deliberately doesn't hide the terminal cursor to improve GDB using experience. The "Release" configuration does the opposite; it doesn't enable debug symbols and hides the terminal cursor.

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