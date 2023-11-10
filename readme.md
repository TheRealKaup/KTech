# KTech

***KTech*** is Kaup's C++ terminal game engine library. It is like any other basic 2D game engine, but its graphics are terminal based.

Table of contents of this readme file:
- [Documentation](#documentation) - links to projects's documentation (intentionally on top for quicker access).
- [Introduction](#introduction) - non-specific, general explanation of what KTech is.
- [Manifesto](#manifesto) - what this project solves, goals of the project.

---

## Documentation

KTech's entire documentation is stored within KTech's GitHub repostiroy (this very one).

In front of you are listed all of the files that comprise KTech's documentation: 

- [tutorials.md](tutorials.md) - tutorials about getting KTech, using its different elements, creating a game with it and compiling.
- [references.md](references.md) - direct information about each element of the engine.
- [examples.md](examples.md) - analyses and explanations of the examples included in the "examples" directory in this repository.
- [faq.md](faq.md) - answers for frequently asked questions.
- [LICENSE](LICENSE) - the license which covers KTech.

---

## Introduction

KTech is a C++ terminal 2D game engine library.

As a game engine, KTech is capable of rendering graphics, playing audio, processing physical objects' movements, handling player input, et cetera.

As a library, KTech offers an ecosystem of types, functions and variables that work together and managed by the user .

Not to be confused, KTech is **not** a software development kit (SDK) like most known game engines are. This means KTech is not an application which requires installation, and after installed can be used to create a game via a graphical user interface (GUI). KTech is nothing but a library - a header file with declerations of the ecosystem's elements, and ".cpp" files with definitions of the ecosystem's elements.

As a terminal software, KTech has standard terminal I/O, which means:

- Graphics are printed into the terminal.
- User input (keyboard presses) are received as escape codes and come in a sequence as if typed - just like how a terminal shell, for example, receives input.

These 2 things make KTech unique. The graphics by default get a unique style and the way input is received is different from how modern game engines receive input, which leads to unique input solutions.

Here is how a very simple game made with KTech can look:

![KTech Small Game](https://github.com/TheRealKaup/TheRealKaup/blob/main/simpleplatform1_10-11-23.png)

---

## Manifesto

### What this Project Solves

_In today's world, it is common for new games to release after most of their development resources were invested into their graphics, instead of their game experiences themselves. This phenomenon causes affected games to lose quality, leading to disappointment among the players. KTech solves this issue by implementing old and primitive graphics that are easy to use, look fine and can't become a huge and complex process in development._

For the longest time, when I had to explain why my project exists, I answered with what it solves, usually as written above.

While it's true that KTech's unique graphical style naturally compensates for lack of visual development and attention to detail from the side of the KTech user, I wouldn't say that this point is of a significance, nor would I say that the entire project exists for some kind of techincal necessity or even as a techincal solution at all in most cases.

This is because for video games, you could use any other game engine that fundamentally enjoys from the benefits of modern I/O, and, of course, is a more developed and advanced project. And, for utility terminal applications, there are much better libraries.

But, for the very niche group of software that are both video games and terminal applications, KTech might be a possible solution. KTech could also be a solution for video games that want some uniqueness, which KTech's graphics and I/O could contribute.

So, as for the question, KTech is a solution for video games needing to be displayed in a terminal, or for video games who wish for a unique element.

### The Principle

- Game development, from creating textures, to coding a game, to compiling a game and running a game, shall be fully possible from the terminal with no limitations.

### Goals

- Become a powerful game engine. To make this explicit, KTech is not as powerful as I wish it to be, at all.
- While not a part of the KTech project itself, create a software development kit around KTech to make KTech game development efficient and as modern as possible, while still remaining in the terminal, as goes the principle.


---

## Capabilities

KTech covers all of the aspects that a basic 2D game engine library should.

KTech is capable of:

- **Graphics**: Rendering terminal graphics with 24 bit range color, opacity and layers.
- **Physics**: Object movement with collision events of pushing, blocking and overlapping, based on different collider types.
- **Audio**: Loading, mixing and playing.
- **UI**: Comprises an expanding collection of widgets.
- **Time**: Game loop ticks and invocations management.
- **Input**: Terminal-optimized user input processing.
- Some other quirks and features.

---

## Intereseting Details About KTech

- KTech is simply a library, but I made an application (using KTech itself) called "TextureCreator" which helps creating terminal cell-based textures, and perhaps in the future more applications similar to this will be created, and a proper software development kit will come to be.
- The game developer (the KTech user) constructs their own game loop with functions that the engine comes with.
- Although KTech's level is quite low, it attempts at being easy to use, friendly and intuitive for new game developers.
- The engine relies only on the CPU for its processing (audio, graphics, collision...) and achieves more than playable performance.
- Making a game with KTech can be done entirely from a terminal by using a terminal text editor (like Vim) for writing code, the TextureCreator application for creating textures, a C++ compiler accessible from a terminal to create an executable, and by simply running the game to play it in the terminal.
- I, Kaup, started developing KTech in March 2022. I had many breaks, sometimes as long as a month. This is the biggest programming project I have ever had, and I wouldn't be surprised to know that until now I have invested a 4 digit number of hours into developing it.

---

## Interrelated Projects

### netset

A KTech game in active development. In the game the player is a freelance hacker that gets jobs from the darknet. The game involves a networking simulation, and the player follows a story line rather then receiving randomly generated jobs. The gameplay consists of searching for information, analyzing it and converting it into new information, until the goal is reached.

Here's a screenshot of the game, from 9.10.2023 (d/m/y):

![netset screenshot from 9.10.2023](https://github.com/TheRealKaup/TheRealKaup/blob/main/netset-9.10.2023.png)

### TextureCreator

An application made with KTech for creating KTech texture files. This program is needed for creating KTech textures because each terminal character cell has an RGB foreground, RGB background and a character values. This kind of texture can't be created by a normal pixel-based image creator.

![TextureCreator Screenshot](https://github.com/TheRealKaup/TheRealKaup/blob/main/texturecreatorscreenshot.png)
