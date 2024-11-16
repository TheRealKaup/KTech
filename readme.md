# *KTech*, Kaup's C++ Terminal Game Engine

- [Introduction](#introduction) - general explanation of what KTech is.
- [Documentation](#documentation) - links to the project's documentation.
- [Building](documentation/faq.md#how-to-build-ktech-with-premake) - detailed building instructions.
- [Running](documentation/faq.md#how-to-run-the-game-examples) - how to run the provided game examples.
- [Capabilities](#capabilities) - key features included in the library.
- [Contributing](#contributing) - how to contribute to the development of KTech.
- [Interrelated Projects](#interrelated-projects) - other projects interrelated with KTech.
- [Contact Information](#contact-information) - how to contact the developer.

![gplv3-or-later](https://www.gnu.org/graphics/gplv3-or-later.png)

---

## Introduction

KTech is a C++ terminal 2D game engine library, available for GNU/Linux and Windows.

As a game engine, KTech is capable of rendering graphics, processing movement of objects, handling player input, and more.

As a library, KTech offers an ecosystem of types, functions and variables that work together to form a game.

Not to be confused, KTech is not a software development kit (SDK), as goes the traditional sense of the term "game engine". KTech is a library; source and header files you include in your game project.

As terminal software, the graphics are printed to the terminal, and user inputs are received as escape codes in a sequence as if typed. This makes KTech games unique graphics-wise and input-wise, as they lead to unconventional solutions.

Making a game with KTech can be done entirely from a terminal: use a terminal text editor (such as Vim) for coding, [TextureCreator](#texturecreator) for creating textures, [Premake and Make for building](/documentation/faq.md#how-to-build-ktech-with-premake), and then, simply run the game.

Here's how a very simple KTech game can look like:

<img src="https://github.com/TheRealKaup/TheRealKaup/blob/main/simpleplatform1_10-12-23.png" alt="KTech Simple Platform Game" max-height="500"/>

The code for this one is in [examples/simpleplatform/game.cpp](examples/simpleplatform/game.cpp).

---

## Documentation

- [`faq.md`](documentation/faq.md) - answers to frequently asked questions **(includes detailed building instructions)**.
- [`tutorial/`](documentation/tutorial/1-introduction.cpp) (work in progress) - comprehensive tutorial conveyed via code and practice.
- Doxygen (work in progress) - library reference, comprising concise information about each element.
- [`COPYING`](COPYING) - the license which covers KTech (GPLv3-or-later).

---

## Capabilities

- **Graphics** - rendering terminal graphics with 24-bit color depth, opacity and layers.
- **Physics** - object movement with pushing, blocking and overlapping events, based on different collider types.
- **UI** - comprises an expanding collection of widgets.
- **Time** - game loop and invocations management.
- **Input** - terminal-optimized user input processing.
- **Memory** - serializable containers for game structures.
- Some other quirks and features.

---

## Contributing

- GitHub issues recommended for new contributers are listed at https://github.com/TheRealKaup/KTech/contribute.
- Please report bugs and suggestions. You can do so in the form of a detailed issue, there's no specific format to comply with.
- You are very welcome to contribute code via a pull request.

---

## Interrelated Projects

### netset

A KTech game in active development. The player is a freelance hacker that gets jobs from the darknet. The game involves a networking simulation and a story line. The gameplay consists of searching for information, analyzing it and converting it into new information, until the goal is reached.

Here's a screenshot of the game, from 9.10.2023 (d/m/y):

<img src="https://github.com/TheRealKaup/TheRealKaup/blob/main/netset-9.10.2023.png" alt="netset screenshot from 9.10.2023" max-height="500"/>

### [TextureCreator](https://github.com/TheRealKaup/TextureCreator)

An application made with KTech for creating cell-based texture files for KTech games. This program is required because each terminal character cell has an RGB foreground color, an RGB background color and a character. This kind of texture can't be created by a normal pixel-based image creator.

<img src="https://github.com/TheRealKaup/TheRealKaup/blob/main/texturecreatorscreenshot.png" alt="TextureCreator Screenshot" max-height="500"/>

---

## Contact Information

For legal and business matters, Kaup (Ethan Kaufman), via email: ethan.kauf736@gmail.com
