# Official KTech Tutorial

Welcome to the official KTech tutorial!

This is my attempt at teaching a subject while putting everything into practice in an interactive fashion. Hopefully, by the end of this tutorial you'll not only know every aspect of KTech, but also have experience with every aspect of KTech. The latter can be as important as the former.

You should clone KTech's git repository and have your own working directory: the best way to complete the tutorial is by changing it. Having your own local source files makes tweaking parameters, rearranging function calls, rewriting classes, and so on, as convenient as possible for you. If you don't understand exactly how a function parameter affects your game: build the tutorial, run, change the parameter, build and run again, and see what it does with your own eyes.

I assume you know how to use the terminal and program some C++ code. When you come across something you don't know (that isn't about to be explained), pause, learn it, and only then come back; don't frustrate yourself and waste your time.

It's recommended that you take a short look and keep in mind the [Doxygen API reference](../../readme.md#documentation) while going through this tutorial. Since the tutorial can't and shouldn't behave like an API reference, you might find some functions and variables referenced here that feel like are missing some fuller explanation. The Doxygen API reference's mission is to provide exactly that.

## Chapters

The tutorial is separated into chapters, each one is a C++ source file that usually contains a heavily-commented example of functional KTech code. You should read the tutorial linearly if you are new to KTech; literally read the individual files from top to bottom, and the chapters in an ascending order.

1. [Introduction](1-introduction.cpp): build and run the tutorial's source files.
2. [World](2-world.cpp): become familiar with basic world hierarchy (`Map`, `Layer`, `Object` and `Camera`).
3. [Game Loop](3-game_loop.cpp): create a proper game loop that updates, renders and prints our world.
4. [Input](4-input.cpp): create an `Object`-inherited character class that registers input callback functions for movement.