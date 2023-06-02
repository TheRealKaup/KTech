# KTech

### (Scroll down for documentation)

## KTech is Kaup's C++ terminal game engine. It is supposed to work like any other basic 2D game engine, but instead of printing its graphics into a graphical window, it prints them into a terminal.

### _In today's world, it is common for new games to release after most of their development resources were invested into their graphics, instead of their game experiences themselves. This phenomenon usually causes such games to not be as good as expected by the players, leading to disappointment among the fan base. KTech solves this issue by implementing old and primitive graphics that are easy to use, look fine and can't become a huge and complex process in the development._

### KTech is currently capable of:

1. Rendering with layers, opacity, 24 bit color range.
2. Processing collision with event handling, collider types, multiple colliders of different types in a single object, pushing, blocking and overlapping.
3. Mixing and playing multiple audio sources, loading .wav files.
4. Basic GUI (input fields, buttons).
5. Loading from files and using special terminal object textures.
6. Handling input.
7. Handling time.
8. Some other quirks and features.


### More information about KTech:

1. It is very object-oriented.
2. It tries to be easy to use.
3. The game developer (the KTech user) constructs their own game loop with functions that the engine comes with.
4. Important classes: Texture, Object, Camera, Layer, Map, AudioSource, TimePoint.
5. For its own processing (audio, rendering, collision...) it uses only the CPU.
6. It can probably be more optimized and fast, but it is generally resource cheap, especially regarding CPU usage.
7. Game development with KTech can be done completely with a terminal by using a terminal text editor (like Vim), the TextureCreator program and a C++ compiler accessible from a terminal.
8. I started developing KTech in March 2022. I had many breaks, sometimes as long as a month. At the times development was more consistent, it was very slow and usually got a couple of hours per week. Whatsoever, this is the biggest programming project I have ever had.

### Other projects interrelated with KTech:

#### TextureCreator
A program made with KTech for creating KTech texture files. This program is needed for creating KTech textures because each terminal character cell (pixel) has an RGB foreground, RGB background and a character values. This kind of texture can't be created by a normal image creator.

#### ThatGame
A game made with KTech that I am working on alongside KTech itself. In the game the player plays as a gifted hacker that gets hacking and heist jobs from the dark web. It is called "ThatGame" because I haven't yet found a name for it.

---
# Documentation

<ins>**KTech's documentation was written to be suitable for people who are new to game development, and even to some computer programming concepts.**</ins>

Section:
- Tutorials
	- Intro: KTech's interface, how to start using it
	- Making a simple platform game
	- Using user interface (UI) widgets
	- How KTech works, looking at the engine's files
	- Creating a custom game loop
- Reference
	- Engine
	- Input
	- Graphics
	- Collision
	- Audio

Notes:
- **I strongly advice that whenever you encounter a programming concept which you are not familiar with, you should pause to learn it.**
- I sometimes use the words "directory" and "folder" interchangeably, as well as "KTech" and "the engine".

To-write list:
- Why KTech so heavily uses pointers

## Tutorials

### Intro: KTech's interface, how to start using it

#### A little about using KTech

In this tutorial you, the KTech game developer, will learn how to start using KTech in order to make a game.

Unlike the big, complex and feature-rich game engines you might be familiar with (like Unity or Unreal Engine), KTech isn't an application that you need to install in order to, for example, drag objects into your game's world, or, run the game to test it.

KTech and KTech games are written with the programming language C++. So, in order to write a KTech game, you need to know some C++.

KTech is a library (simply code) that you put in your game's code directory. KTech's code has an application programming interface (API) in the form of a C++ header file (.hpp file), which you can include in your game's code in order to access KTech's functionality.

And, whenever you want to run your game, you need to manually compile it (turn code into a runnable executable with a compiler program of your choice, such as G++) and run the executable. This is in contrast to something like the run button in Unity or Unreal Engine.

#### How to actually setup KTech game development:
1. Download the "engine" folder from the KTech GitHub repository.
2. Put the engine folder in a directory which will be your game's code directory.
3. Create a C++ code file in your game's code directory, and call it whatever you'd like. For example, "game.cpp".
4. Include the KTech library in your game's code files by writing `#include <engine/engine.hpp>` at the start of the code.

#### So, what exactly did we do here?
If you wish to understand, this part is for you.

The "engine" folder is the KTech game engine's code, and it is important to understand that KTech is nothing more than that. KTech isn't a software you need to open in order to create a world with logic and rules. KTech is a collection of tools, expressed as code, that make game development easier and more accessible, or in other words, a library.

By writing `#include <other_code_file_name.hpp>` in a code file, you basically get access to its content. I enourage you to check the content of the file "engine.hpp" in the engine directory. You don't have to understand what everything there means, but notice the things such as `Map`, `AudioSource`, or `InitializeAudio`. These are all the concepts, tools or however you'd like to call them, which the engine provides. The other code files in the folder define the functionality of these functions.

So, by adding `#include <engine/engine.hpp>` at the start of your game's code files, you are accessing the concepts and functions in that C++ header file (.hpp), which are defined in the other C++ code files (.cpp).

#### About KTech classes

Concepts, such as a map, an object and a camera are classes.

In the making of your game, you create instances of these classes.

It is important to understand that unlike other common game engines, you are responsible for creating, keeping and changing instances of these classes, and not, for example,  a physics system that automatically updates your objects position according to gravity, or, a drag and drop experience to set the positions of cameras and objects at the start of the game. And also, you can always go deeper and remove or replace KTech functionality with your own, in case you feel like you still don't have enough control.

It might sound like game development with KTech is very manual and tedious, but fear not, because the engine provides functions that make it easy to program your game's functionality, which saves you writing compelx and long code, but doesn't save you from pretty much infinite control over your game. This is a considerable advantage over common game engines, which either hide their code and functionality, or are too big and complicated to understand, analyze, and modify.

#### How to actually access classes and engine functions

If you look inside `engine.hpp`, you will see that everything is written within a "namespace". A namespace is used to organize code, and make things clearer especially when you use the namespace's content.

In order to reach an engine class, for example, the "Map" class, you have to access it from the namespace, by specifying the namespace.

Let's create a map:

`game.cpp`
```c++
#include <engine/engine.hpp>

int main()
{
	Engine::Map map;
}
```
Here, the game starts from the usual C++ entry point, `main()`. We use the `Map` class from the `Engine` namespace we got access to by including in the first line the "engine.hpp" header file from the "engine" folder we downloaded into our game development directory.

This program, or "game", by the way, creates an empty map an immediately exists and ends.

#### Comparing ways of doing things with KTech and the usual game engine

If you come from other game engines, like Unity or Unreal Engine, the code from the last section is pretty much what these engines do at the start of a game if you told them in the game making editor that there is a map.

This is the core difference between the game development experiences, the fact that there is no "editor" for this kind of stuff in KTech.

*This actually might change in the future. Just like how there is TextureCreator (an application made with KTech for creating KTech game textures) for making KTech game development easier, there can be more of these applications to create, for example, maps and animations.

Let's expand the program from before and also add a layer to the map, and an object to that layer, and a texture to that object.

A layer, by the way, allows to seperate objects in the same map if you wish so. For example, you can have a layer for the ground and a layer for clouds.

Step 1: create a layer, and add the layer to the map.

`game.cpp`
```c++
#include <engine/engine.hpp>

int main()
{
	Engine::Map map; // Create an empty map
	
	Engine::Layer layer; // Create an empty layer
	map.AddLayer(&layer); // Add the layer to the map
}
```

As you can see, with KTech, when you add things to other things you usually add them as pointers. It is very important to know what pointers are and how to use them, so if you don't feel comfortable using them, please pause reading this and return after you have learned how to use them properly. If you are curious to know why KTech uses pointers pointers so heavily, there is a section explaining this.

Step 2: create an object.

`game.cpp`
```c++
#include <engine/engine.hpp>

int main()
{
	Engine::Map map; // Create an empty map
	
	Engine::Layer layer; // Create an empty layer
	map.AddLayer(&layer); // Add the layer to the map
	
	Engine::Object object( // Create an object
		Engine::Vector2D(5, -5), // Set the position
		"My Object"); // Set the object's name
}
```

Here, we created an object and used its class constructor. `Object` has 2 optional parameters, meaning you don't have to fill them, and instead use the default values which will set the object's position to (0, 0) and the name to "" (empty). `Engine::Vector2D` is another class like `Map`, `Layer` and `Object`, which has 2 variables inside of it: an X value and a Y value. So, so far we have an object that is at the position of (5, -5) and is called "My Object".

Step 3: add a texture to the object, and add the object to the layer.

`game.cpp`
```c++
#include <engine/engine.hpp>

int main()
{
	Engine::Map map; // Create an empty map
	
	Engine::Layer layer; // Create an empty layer
	map.AddLayer(&layer); // Add the layer to the map
	
	Engine::Object object( // Create an object
		Engine::Vector2D(5, -5), // Set the position
		"My Object"); // Set the object's name

	object.textures.resize(1); // Add space to the texture array
	object.textures[0].Rectangle( // Draw a rectangle shape
		Engine::UVector2D(3, 3), // The size of the rectangle
		Engine::SuperChar('#', Engine::RGBA(0, 0, 255, 1.0f)), // The value each character cell
		Engine::Vector2D(0, 0)); // The position of the texture relative to the object
	
	layer.AddObject(&object); // Add the object to the layer
}
```

I encourage you to look inside the engine header file, and try to grasp what the classes are, have, and their capabilities.
As you can see, `Object` has a vector, which is similar to an array, of textures. Meaning a single object can have multiple textures, which is useful for managing how the object looks. `Texture` is a class of its own, and it has the functions `Rectangle()`, `File()` and `Write()`. These 3 functions are to fill the texture with, well, a texture. `Rectangle()` will make it a rectangle with the same value on each pixel (character cell with a foreground color, background color and character), `File()` will load the texture from a texture file, and `Write()` is for writing the texture in code, which can also be useful sometimes.

So, by resizing the texture vector to 1, we add space to 1 texture.

After resizing it, we can access it from the texture vector as you do with an array, and we will then call Rectangle() to make it a blue 3 by 3 square made of the character '#'.

And lastly, after creating our beautiful object, we add it to the layer.

By the way, because we add the object to the layer as a pointer, we can add the object before giving it a texture and the object that will end up in the layer will still have the later-added texture. So, if you want to organize your code to look a little nicer, you can add all the objects to the layers and the layers to the maps at the top of your code and then initialize it all.

But, like the previous program, it really isn't a game because immediately after we make our world we reach the end of the program, which quits it.

In the next tutorial, we will make a game that will actually run, display our world and even play some sound.