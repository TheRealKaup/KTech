# KTech

***KTech*** is Kaup's C++ terminal game engine library.

Table of contents:
- [Introduction](#introduction) - general explanation of what KTech is.
- [Capabilities](#capabilities) - key features included in the library. 
- [Interesting Details](#interesting-details) - some specifics that could arouse your curiosity.
- [Documentation](#documentation) - links to the projects's documentation.
- [Contributing](#contributing) - How to contribute to the development of KTech.
- [Contact Information](#contact-information) - how to contact me (Kaup).
- [Interrelated Projects](#interrelated-projects) - other projects interrelated with KTech.

![gplv3-or-later](https://www.gnu.org/graphics/gplv3-or-later.png)

---

## Introduction

KTech is a C++ terminal 2D game engine library, currently available for GNU/Linux.

As a game engine, KTech is capable of rendering graphics, playing audio, processing movement of objects, handling player input, and more.

As a library, KTech offers an ecosystem of types, functions and variables that work together and managed by the user.

Not to be confused, KTech is not a software development kit (SDK) like most known game engines are. This means KTech is not an application that comes with a graphical user interface, and rather, it's library; source files and header files you include in your game project.

As terminal software, the graphics are printed to the terminal, and user inputs are received as escape codes and come in a sequence as if typed. This makes KTech games unique graphics-wise and input-wise, as they lead to unconventional solutions.

Here's how a very simple KTech game can look like:

![KTech Small Game](https://github.com/TheRealKaup/TheRealKaup/blob/main/simpleplatform1_10-12-23.png)

---

## Capabilities

- **Graphics** - Rendering terminal graphics with 24-bit color depth, opacity and layers.
- **Physics** - Object movement with pushing, blocking and overlapping events, based on different collider types.
- **Audio** - Loading, mixing and playing.
- **UI** - Comprises an expanding collection of widgets.
- **Time** - Game loop and invocations management.
- **Input** - Terminal-optimized user input processing.
- **Memory** - Serializable containers for game structures.
- Some other quirks and features.

---

## Interesting Details

- KTech is simply a library, which naturally makes certain aspects of KTech game development tedious without external help. For this reason I made an application, using KTech itself, called "TextureCreator" that helps creating terminal-based textures. TextureCreator covers and improves one aspect of KTech game development, and in the future will be part of a full software development kit.
- The game developer (the KTech user) constructs their own game loop with functions provided by the library. While this makes the distinction between game and engine different from popular game engines, this is the design I intentionally chose, because it gives the KTech user stronger control and understanding of what's happening.
- The engine relies only on the CPU for its processing (audio, graphics, collision...) and achieves more than playable performance.
- Making a game with KTech can be entirely done from a terminal by using a terminal text editor (like Vim) for writing code, the TextureCreator application for creating textures, a C++ compiler accessible from a terminal, and by simply running the game to play it in the terminal.
- I, Kaup, started the development of KTech in March 2022. While progress was inconsistent, this is the biggest programming project I have ever worked on, and I wouldn't be surprised to know that until now I have invested a 4 digit number of hours into it.

---

## Documentation

- [quickstart.md](documentation/quickstart.md) - quick start guide if you want to experiment with KTech right away.
- [reference.md](documentation/reference.md) - (will exist in the future) direct and concise information about each element of the library.
- [tutorials.md](documentation/tutorials.md) - (will exist in the future) tutorials about getting KTech, using its different elements, creating a game with it and compiling.
- [faq.md](documentation/faq.md) - (will exist in the future) answers for frequently asked questions.
- [examples.md](documentation/examples.md) - (will exist in the future) analyses and explanations of the examples included in the "examples" directory in this repository.
- [LICENSE](COPYING) - the license which covers KTech (GPLv3-or-later).

---

## Contributing

\<to be written>

Useful link - https://github.com/TheRealKaup/KTech/contribute

---

## Contact Information

Kaup (Ethan Kaufman):
- Email: ethan.kauf736@gmail.com

---

## Interrelated Projects

### netset

A KTech game in active development. The player is a freelance hacker that gets jobs from the darknet. The game involves a networking simulation and a story line. The gameplay consists of searching for information, analyzing it and converting it into new information, until the goal is reached.

Here's a screenshot of the game, from 9.10.2023 (d/m/y):

![netset screenshot from 9.10.2023](https://github.com/TheRealKaup/TheRealKaup/blob/main/netset-9.10.2023.png)

### TextureCreator

An application made with KTech for creating cell-based texture files for KTech games. This program is required because each terminal character cell has an RGB foreground color, an RGB background color and a characterst programming project I have ever had, and I wouldn't be surprised to know that until now I have invested a 4 digit number of hours into making it.st programming project I have ever had, and I wouldn't be surprised to know that until now I have invested a 4 digit number of hours into making it.. This kind of texture can't be created by a normal pixel-based image creator.

![TextureCreator Screenshot](https://github.com/TheRealKaup/TheRealKaup/blob/main/texturecreatorscreenshot.png)
