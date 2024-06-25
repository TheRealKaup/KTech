# *KTech*, Kaup's C++ Terminal Game Engine Library

- [Introduction](#introduction) - general explanation of what KTech is.
- [Documentation](#documentation) - links to the project's documentation.
- [Capabilities](#capabilities) - key features included in the library. 
- [Interesting Details](#interesting-details) - some specifics that could arouse your curiosity.
- [Contributing](#contributing) - how to contribute to the development of KTech.
- [Contact Information](#contact-information) - how to contact me (Kaup).
- [Interrelated Projects](#interrelated-projects) - other projects interrelated with KTech.

![gplv3-or-later](https://www.gnu.org/graphics/gplv3-or-later.png)

---

## Introduction

KTech is a C++ terminal 2D game engine library, currently available for GNU/Linux.

As a game engine, KTech is capable of rendering graphics, processing movement of objects, handling player input, and more.

As a library, KTech offers an ecosystem of types, functions and variables that work together to form a game.

Not to be confused, KTech is not a software development kit (SDK), as goes the traditional sense of the term "game engine". KTech is a library; source and header files you include in your game project.

As terminal software, the graphics are printed to the terminal, and user inputs are received as escape codes in a sequence as if typed. This makes KTech games unique graphics-wise and input-wise, as they lead to unconventional solutions.

Here's how a very simple KTech game can look like:

<img src="https://github.com/TheRealKaup/TheRealKaup/blob/main/simpleplatform1_10-12-23.png" alt="KTech Simple Platform Game" width="400"/>

The code for this one is in [examples/simpleplatform/game.cpp](examples/simpleplatform/game.cpp).

---

## Documentation

- **[quickstart.md](documentation/quickstart.md)** - quick start guide if you want to experiment with KTech right away **(includes building)**.
- **[faq.md](documentation/faq.md)** - answers for frequently asked questions.
- **Doxygen** (work in progress) - library reference, comprising concise information about each element.
- **[COPYING](COPYING)** - the license which covers KTech (GPLv3-or-later).

---

## Capabilities

- **Graphics** - Rendering terminal graphics with 24-bit color depth, opacity and layers.
- **Physics** - Object movement with pushing, blocking and overlapping events, based on different collider types.
- **UI** - Comprises an expanding collection of widgets.
- **Time** - Game loop and invocations management.
- **Input** - Terminal-optimized user input processing.
- **Memory** - Serializable containers for game structures.
- Some other quirks and features.

---

## Interesting Details

- KTech is simply a library, which naturally makes certain aspects of KTech game development tedious without external help. For this reason I made an application, using KTech itself, called "TextureCreator" that helps creating terminal-based textures. TextureCreator covers and improves one aspect of KTech game development, and in the future may be part of a full software development kit.
- The game developer (the KTech user) constructs their own game loop with functions provided by the library. While this makes the distinction between "game" and "engine" different from popular game engines, it give the user wider control and better understanding of what's happening.
- KTech knows no more than the CPU, yet achieves more than playable performance.
- Making a game with KTech can be entirely done from a terminal: use a terminal text editor (such as Vim) for writing code, the TextureCreator application for creating textures, Premake and Make for building. Then, simply run the game.
- I, Kaup, started the development of KTech in March 2022. While progress was inconsistent, this is the biggest programming project I have ever worked on, and I wouldn't be surprised to know that until now I have invested a 4 digit number of hours into it.

---

## Contributing

- GitHub issues recommended for new contributers are listed at https://github.com/TheRealKaup/KTech/contribute.
- Please report bugs and suggestions. You can do so in the form of a detailed issue, there's no specific format to comply with.
- You are very welcome to contribute code via a pull request.

---

## Contact Information

Kaup (Ethan Kaufman):
- Email: ethan.kauf736@gmail.com
- Instagram: https://www.instagram.com/kaufboi/

---

## Interrelated Projects

### netset

A KTech game in active development. The player is a freelance hacker that gets jobs from the darknet. The game involves a networking simulation and a story line. The gameplay consists of searching for information, analyzing it and converting it into new information, until the goal is reached.

Here's a screenshot of the game, from 9.10.2023 (d/m/y):

![netset screenshot from 9.10.2023](https://github.com/TheRealKaup/TheRealKaup/blob/main/netset-9.10.2023.png)

### TextureCreator

An application made with KTech for creating cell-based texture files for KTech games. This program is required because each terminal character cell has an RGB foreground color, an RGB background color and a character. This kind of texture can't be created by a normal pixel-based image creator.

Git repository - https://github.com/TheRealKaup/TextureCreator.

![TextureCreator Screenshot](https://github.com/TheRealKaup/TheRealKaup/blob/main/texturecreatorscreenshot.png)
