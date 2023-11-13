# KTech

***KTech*** is Kaup's C++ terminal game engine library. It is like any other basic 2D game engine, but its graphics are terminal based.

Table of contents of this readme file:
- [Introduction](#introduction) - non-specific, general explanation of what KTech is.
- [Capabilities](#capabilities) - general list of features included in the library. 
- [Intereseting Details](#intereseting-details) - some specifics that could arouse your curiosity.
- [Documentation](#documentation) - links to projects's documentation.
- [Manifesto](#manifesto) - what this project solves, goals of the project.
- [Interrelated Projects](#interrelated-projects) - other projects that are very connected to KTech.

---

## Introduction

KTech is a C++ terminal 2D game engine library.

As a game engine, KTech is capable of rendering graphics, playing audio, processing physical objects' movements, handling player input, et cetera.

As a library, KTech offers an ecosystem of types, functions and variables that work together and managed by the user.

Not to be confused, KTech is not a software development kit (SDK) like most known game engines are. This means KTech is not an application which requires installation, and after installed can be used to create a game via a graphical user interface (GUI). KTech is nothing but a library - a header file with declerations of the library's ecosystem's elements, and ".cpp" files with definitions of the elements.

As a terminal library, KTech has standard terminal I/O, which means:

- Graphics are printed into the terminal.
- User input (keyboard presses) are received as escape codes and come in a sequence as if typed - just like how a terminal shell, for example, receives input.

These 2 things make KTech a unique game engine. The graphics by default get a unique style and the way input is received is different from how modern game engines receive input, which leads to unique input solutions.

Here is how a very simple game made with KTech can look:

![KTech Small Game](https://github.com/TheRealKaup/TheRealKaup/blob/main/simpleplatform1_10-11-23.png)

---

## Capabilities

KTech covers all of the aspects that a basic 2D game engine library should.

KTech is capable of:

- **Graphics**: Rendering terminal graphics with 24 bit range color, opacity and layers.
- **Physics**: Object movement with collision events of pushing, blocking and overlapping, based on different collider types.
- **Audio**: Loading, mixing and playing.
- **UI**: Comprises an expanding collection of widgets.
- **Time**: Game loop and invocations management.
- **Input**: Terminal-optimized user input processing.
- Some other quirks and features.

---

## Intereseting Details

- KTech is simply a library, which naturally makes certain aspects of KTech game development tedious without external help. For this reason I made an application, using KTech itself, called "TextureCreator" that helps creating terminal cell-based textures. TextureCreator covers and improves one aspect of KTech game development, and in the future will be a part of a full software development kit.
- The game developer (the KTech user) constructs their own game loop with functions provided by the library. While this makes the distinction between game and engine different from popular game engines, this is the design I intentionally chose, because it gives the KTech user superior control and understanding of what's happening.
- The engine relies only on the CPU for its processing (audio, graphics, collision...) and achieves more than playable performance.
- Making a game with KTech can be done entirely from a terminal by using a terminal text editor (like Vim) for writing code, the TextureCreator application for creating textures, a C++ compiler accessible from a terminal to create an executable, and by simply running the game from a shell to play it in the terminal.
- I, Kaup, started the development of KTech in March 2022. I had many breaks, sometimes as long as a month. This is the biggest programming project I have ever had, and I wouldn't be surprised to know that until now I have invested a 4 digit number of hours into making it.

---

## Documentation

KTech's entire documentation is stored within KTech's GitHub repostiroy (this very one).

In front of you are listed all of the files that comprise KTech's documentation: 

- [references.md](references.md) - direct information about each element of the library.
- [tutorials.md](tutorials.md) - tutorials about getting KTech, using its different elements, creating a game with it and compiling.
- [faq.md](faq.md) - answers for frequently asked questions.
- [examples.md](examples.md) - analyses and explanations of the examples included in the "examples" directory in this repository.
- [LICENSE](LICENSE) - the license which covers KTech.

---

## Manifesto

### What this Project Solves

_In today's world, it is common for new games to release after most of their development resources were invested into their graphics, instead of their game experiences themselves. This phenomenon causes affected games to lose quality, leading to disappointment among the players. KTech solves this issue by implementing old and primitive graphics that are easy to use, look fine and can't become a huge and complex process in development._

For the longest time, when I had to explain why my project exists, I answered with what it solves, usually as written above.

While it's true that KTech's unique graphical style naturally compensates for lack of visual development and attention to detail from the side of the KTech user, I wouldn't say that this point is significant, nor would I say in the first place that the project exists for some kind of techincal necessity or even as a superioir techincal solution in most cases.

This is because for video games, you could use one of the many popular game engines that are fundamentally better because of the modern I/O they benefit from, and of course not forgetting the simple fact that they are more capable and supported. And, for utility terminal applications, there are much better libraries that focus on UI.

But, for the very niche group of software that are both terminal applications and video games, KTech can be quite a good solution since it is a terminal library that is specifically designed to support video games, meaning it includes features such as physics, audio, game loop time management, et cetera.

Additionally, KTech could be an interseting solution for video games that weren't originally planned to be terminal applications, and want some uniqueness, which KTech's I/O could definitely contribute.

So, as for the question, KTech is a solution for video games that need to be displayed in a terminal, or, for video games wishing for a unique element.

### The Principle

Game development, from creating textures, to coding a game, to compiling a game to running a game, shall be fully possible from a terminal with no limitations.

### Goals

- Become a powerful game engine. To make this explicit, KTech is not as powerful as I wish it to be, at all.
- Though to mention not a part of the KTech project itself; create a software development kit around KTech to make KTech game development efficient and as modern as possible, while still remaining in the terminal, as goes the principle.

---

## Interrelated Projects

### netset

A KTech game in active development. In the game the player is a freelance hacker that gets jobs from the darknet. The game involves a networking simulation, and the player follows a story line rather then receiving randomly generated jobs. The gameplay consists of searching for information, analyzing it and converting it into new information, until the goal is reached.

Here's a screenshot of the game, from 9.10.2023 (d/m/y):

![netset screenshot from 9.10.2023](https://github.com/TheRealKaup/TheRealKaup/blob/main/netset-9.10.2023.png)

### TextureCreator

An application made with KTech for creating cell-based texture files for KTech games. This program is required because each terminal character cell has an RGB foreground color, an RGB background color and a character values. This kind of texture can't be created by a normal pixel-based image creator.

![TextureCreator Screenshot](https://github.com/TheRealKaup/TheRealKaup/blob/main/texturecreatorscreenshot.png)
