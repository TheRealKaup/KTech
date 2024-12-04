# Outline

This document contains answers for ~~frequently~~ potentially asked questions.

- [How to build KTech?](#how-to-build-ktech-with-premake)
- [How to build your own KTech game (with Premake)?](#how-to-build-your-own-ktech-game-with-premake)
- [How to run the game examples?](#how-to-run-the-game-examples)
- [How does the licensing work?](#how-does-the-licensing-work)
- [Is KTech Stable?](#is-ktech-stable)
- [Why are `Collider`'s and `Texture`'s constructors normal functions?](#why-are-colliders-and-textures-constructors-normal-functions)
- [Why is there no predefined game loop?](#why-is-there-no-predefined-game-loop)
- [How does the file system work?](#how-does-the-file-system-work)
- [How does `CachingRegistry` work?](#how-does-cachingregistry-work)
- [KTech's history with the Windows Console and the POSIX terminal](#ktechs-history-with-the-windows-console-and-the-posix-terminal)
- [Will there be a software development kit?](#will-there-be-a-software-development-kit)
- [Is KTech GPU-accelerated?](#is-ktech-gpu-accelerated)
- [Why is the library named "KTech"?](#why-is-the-library-named-ktech)

# Q&A

## How to build KTech (with Premake)?

KTech uses **Premake** to configure and generate build files, because it has [fine documentation](https://premake.github.io/docs/) and is configured with the normal scripting language Lua, unlike CMake with its counterintuitive documentation and odd scripting language.

To generate the build files for KTech and the game examples in the `examples/` directory, ensure Premake is installed on your system and run the command `premake5 [action]` from the Git repository's root (e.g. `premake5 gmake2` to generate GNU Makefiles).

To then generate binary files with GNU Make, run the command `make -C build/`, and they will be outputted to the directory `build/bin/`.

## How to build your own KTech game (with Premake)?

If you aren't familiar with Premake I recommend reading the "Writing Premake Scripts" or at least the "Getting Started" sections of its wonderfully concise [documentation](https://premake.github.io/docs/).

The library's [`ktech/premake5.lua`](/ktech/premake5.lua) script/configuration file instructs Premake to create a static library Premake project named `KTechLibrary`.

To link the library, you will have to create your own `premake5.lua` script that should do the following things:
- Create a Premake workspace.
- Include `ktech/premake5.lua`.
- Create a `ConsoleApp` Premake project for your game that links `KTechLibrary`.

For example (`--` in Lua precedes a comment):

```lua
workspace "KTechGame" -- this name doesn't matter to KTech
	configurations { "Debug", "Release" }
	location "build" -- where to generate build files (doesn't matter to KTech)

include "ktech/" -- KTech's directory, which contains its `premake5.lua` script

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

To generate the build files for KTech and your game, ensure Premake is installed on your system and run the command `premake5 [action]` from the directory your `premake5.lua` script is in (e.g. `premake5 gmake2` to generate GNU Makefiles).

To then generate binary files with GNU Make, run the command `make -C build/`, and they will be outputted to `build/bin/`.

The "Debug" configuration (on KTech's end) enables debug symbols and intentionally doesn't hide the terminal cursor to improve GDB using experience. The "Release" configuration leaves debug symbols disabled and hides the terminal cursor.

## How to run the game examples?

There are 2 game examples, "simpleplatform", which tests collision and various graphical features, and "widgetstest", which tests all the UI widgets available.

Running the Premake script, as described in "[How to build KTech (with Premake)?](#how-to-build-ktech-with-premake)", will build these 2 game examples. The "widgetstest" binary (which will be outputted to `build/bin/widgetstest`) can be ran from anywhere, though the adjacent `build/bin/simpleplatform` binary should be ran from its source directory (`examples/simpleplatform/`), so it could load its texture files.

For an example of a purposed UI program made with KTech, see [TextureCreator](https://github.com/TheRealKaup/TextureCreator).

## How does the licensing work?

KTech is licensed under the [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.en.html) (GPLv3) or any later version. Read the article "[A Quick Guide to GPLv3](https://www.gnu.org/licenses/quick-guide-gplv3.html)" from GNU's website to learn about it. You are completely allowed to sell copies of your game (for example, via Steam), and so does anyone else. How significantly this affects sales in practice is unclear.

The Steamworks API might be compatible with KTech (to utilize Steam achievements, for instance), under certain interpretations of the GPLv3. This might be true as long as it's implemented in a modular, optional and replaceable manner. An abstraction layer might be added to KTech to support this in the future.

## Is KTech Stable?

KTech is reasonably uncomplicated and is written under pretty rigid code conventions (see [style.md](style.md)). Despite so, I frequently encounter bugs. This is very much not to be unexpected, as KTech has seen much more coding done on it than actual usage. Parallel to the last time I've worked on my game _netset_, I've composed what would seem a worryingly long list of overlooked parts (see [issue #58](https://github.com/TheRealKaup/KTech/issues/58)). Solving most of them was as easy as finding them. The asnwer to the question: no. But, what I'm propounding is that evidently, KTech is not fundamentally faulty, and rather it needs more testing and feedback.

## Why are `Collider`'s and `Texture`'s constructors normal functions?

Using normal constructors was ambiguous to the coder and the compiler in this case. The writing convention of `vector.resize(n)` and `vector[i].Constructor()`, where "vector" is a vector of `Texture`s or `Collider`s, is just the simple and easy alternative I found. I might change this method, maybe to a factory or a template-based design.

## Why is there no predefined game loop?

It seems to me preferable forcing the user to write a game loop and understand how it works. Depends on how extensible a predefined game loop should be, it might require an amount of parameters and new features that is equivalent in code complexity to writing a game loop on the game side as is done now, which I believe is not too inconveniencing anyway.

## How does the file system work?

KTech is provided as a static library. It's declared and defined within [the `ktech/` directory of this repository](https://github.com/TheRealKaup/KTech/tree/master/ktech).

The core library is declared in `ktech/ktech.hpp`. This file is what your game source files should `#include`.

`ktech.hpp` contains forward declarations, all of which are enclosed within a namespace named `KTech`. Classes are defined in the other header files (`*.hpp`), and class members are usually defined in additional source files (`*.cpp`).

The non-core parts of the library are so far only the optional predefined UI elements ("widgets") that are stored in `ktech/widgets/`. If you want to use any of KTech's widgets, you may selectively include the header files from that directory.

The contents of `ktech/` is split into multiple sub-directories:
- `basic/` - basic classes.
- `engine/` - the `Engine` class and engine components.
- `utility/` - utility classes.
- `widgets/` - optional widgets that you may include from here.
- `world/` - world classes.

Note that `ktech.hpp` includes all of the required header files only if `KTECH_DEFINITION` is undefined (it is indeed normally undefined). This macro is an include guard completely internal to KTech, and you shouldn't define it before including `ktech.hpp`. Apart from that, don't worry about this macro, unless you want to know more about it in the entry "[What is the `KTECH_DEFINITION` macro?](#what-is-the-ktech_definition-macro)"

## How does `CachingRegistry` work?

`KTech::ID` is initialized with a UUID value when constructed, meaning, references to world structures (classes that have an `ID m_id` member) are serializable. All that is left is some kind of a container to store world structure instances or register world structure pointers, so world structures can be retrieved using their `ID`s.

I chose to register pointers rather than store instances, and `KTech::CachingRegistry` is implementation. Its purpose is to retrieve a registered world structure pointer, given a corresponding UUID. As its name suggests, it doesn't simply iterate through all registered world structures and retrieves the one that has the matching UUID, but rather, it uses some kind of caching-related optimization.

`KTech::ID` doesn't only comprise a UUID value: it also has an index (`ID::m_i`). This member is used by `CachingRegistry` to cache the last known index in the registry of the associated world structure. When `CachingRegistry` attempts to retrieve a world structure pointer, it first checks if the UUID of the given `ID` matches the UUID of the world structure registered at the given cached index. Optimally they match, so it simply returns the pointer from the cached index. If they don't, it iterates down the registry until it finds the matching UUID (i.e. the wanted world structure).

`CachingRegistry` will update the cached index of any `ID` it receives, expediting future usages of that `ID`. The cached index of `ID` is declared `mutable`, which is how `CachingRegistry` can modify it even though it asks for const-references of `ID`s.

Additionally, considering KTech is designed to work single threaded, keeping a direct pointer to a world structure retrieved from `CachingRegistry` for the duration of a single function, after validating it (using `CachingRegistry::Exists()` or checking that the returned pointer is not `nullptr`), is valid practice, as nothing external should erase the pointed world structure from memory in the meantime.

## KTech's history with the Windows Console and the POSIX terminal

KTech was initially written for Windows in 2022. At least a year later I moved it to GNU/Linux, which forced upon it a wide set of changes rooted in the understanding of the terminal utility.

In Windows, you can basically do anything. You can use the `GetAsyncKeyState()` function from the Windows API, which serves an ideal method to get key states and detect key presses for a game engine. When the game starts, you can take one of the many approaches in Windows to resize the Console to fit the game's viewport, such as by finding its window handle, setting its size and locking it there, or something, I don't really remember. The functions from the `Windows.h` library didn't always functioned as expected and sometimes you had to look for odd workarounds after frustrating hours of trial and error, but nonetheless, the KTech I had back then behaved like an actual modern game, one that would take dominion of the section on the screen that is simply supposed to display it. KTech was not terminal software, it was Windows Console software. Though to be fair, I called it a "Console" game engine, KCGE; Kaup's Console Game Engine, with an intentional reference to the Windows Console.

Switching to GNU/Linux, I started porting my project to the Unix (POSIX) terminal. I remember totally not conceptualizing what differs in this new environment, so I went on a journey to make each terminal capability that worked on Windows, work on GNU/Linux, in the exact same way. Okay, input. Normal terminal input is received in a sequence as if typed, like when you open a text editor and hold the letter "a"; it inputs "a", and after a certain delay it spams "a". This isn't what you want for a game, so I had to track down the "`GetAsyncKeyState()` of Linux". That isn't really a thing, unless you count reading the actual keyboard device file, which is something I tried, and didn't implement because it required root access. I then learned about X11 and tried utilizing it, to get input and also window management up and running. I quit before I got anything working.

KCGE was no longer named that; the goal I set was to make an actual terminal game engine. I remembered that as I was installing Arch Linux on my system I was given a terminal, but no X11. It means KTech games couldn't be played on a Linux TTY, deeming them X11 programs rather than terminal programs. Yet that's not all of it. What if someone launches the game on Wayland? I'll have to verify XWayland manages to handle my library. Well, what if the player is using a window manager that doesn't expect the terminal to normally, and randomly, resize itself? What if changing the font doesn't work the same across all terminal emulators? But undoubtedly, the crux of it all: what if the user is expecting the terminal application to behave like a terminal application?

It was made apparent to me that KTech is not to besiege the terminal, but rather the opposite. The terminal's size, font, inputs, location on the screen; these are all under the responsibilities of the terminal and the window manager, which are themselves under the user's control. Violating this idea ultimately violates the user's rightful sovereignty over their personal computer. For example, a terminal program ought to adapt to the terminal's displayed buffer, in lieu of resizing the terminal to fit the program. Alas, the standardized terminal still has some limitations that if broken won't necessarily contradict this view, notably, removing delay before keys start repeating, locally, in the terminal, is impossible as far as I know.

Over a year later I returned the Windows port, because I have big plans for KTech and the games I'm gonna make with it. This time the currently installed doctrine was kept (obviously, as I want games to behave the same between Windows and GNU/Linux), but almost: selecting text in the Windows Console strangely pauses output, which is more annoying than ideological, so that feature, on the Windows port, I decided to disable.

## Will there be a software development kit?

Personally I haven't directly planned on writing more SDK-like components similar to [TextureCreator](https://github.com/TheRealKaup/TextureCreator), though I will expand TextureCreator's capabilities as I return to developing my game netset. If someone wants to take the engine for a ride and make another tool with it, I'd be honored.

## Is KTech GPU-accelerated?

KTech doesn't know more than the CPU, but it should be clear that some terminal has to display its prints. Such terminal is likely rendered by a GPU, and also could be itself a GPU-accelerated terminal emulator.

Nonetheless, KTech achieves beyond playable performance.

## What is the `KTECH_DEFINITION` macro?

The `KTECH_DEFINITION` macro is defined in KTech's header files, in order to include the needed `KTech` namespace, while avoiding all of the header files below it. The reason this is needed is best explained with an example.

Assume we have the `Output` engine component defined in `output.hpp`:

```cpp
#include "../ktech.hpp"

class KTech::Output
{
	// ...
}
```

And `Engine` defined in `engine.hpp`:

```cpp
class KTech::Engine
{
	// ...
	Output output;
	// ...
}
```

If `engine.hpp` would be included alongside the `KTech` namespace by including `ktech.hpp` in `output.hpp`, the preprocessor will create something like this:

```cpp
namespace KTech
{
	// ...
	class Output;
	class Engine;
	// ...
}

class KTech::Engine
{
	// ...
	Output output;
	Engine() : output(this) {}
	// ...
}

class KTech::Output
{
	// ...
}
```

The problem here is that `Output` is still undefined in `Engine` (not to mention that its constructor isn't even declared). Meaning, the creation of an `Output` instance in `Engine` prompts an error (to the compiler, it is an incomplete type).

To solve this, the `KTECH_DEFINITION` macro is used as an include guard to prevent the inclusion of `engine.hpp`.

This would be simpler if the namespace and the `#include` directives were separated into 2 files. That way, the user would include a header with all the `#include` directives, and KTech's code base would include a header with just the namespace. But, I wanted to have both in one place, so when a user looks at the contents of the file they include in their game (`ktech.hpp`), they immediately see what types and variables they get and may choose.

## Why is the library named "KTech"?

Originally I called it KCGE, which meant "Kaup's Console Game Engine". As part of opening its codebase, I renamed it to KTech. The 'K' remained, and with "Tech" concatenated, this project truly is an AliExpress brand. It's corny and retro, and I think it's fitting.