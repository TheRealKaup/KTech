# Table of Contents:

- Table of Contents
- Introduction to Quick Start Guide
- Key Concepts
- Starting a Project
- Building

# Introduction to Quick Start Guide

This document is a short tutorial on using KTech. Note that it covers only a small portion of the library and minimally explains how to use it.

If you are looking for a concise tutorial, or the library reference, refer to the [documentation section in the `readme.md` file](https://github.com/TheRealKaup/KTech/blob/master/readme.md#documentation) (both aren't available yet, and as for now this tutorial is KTech's entire documentation).

# Key Concepts

## The File System

The entire library is in [the `ktech` directory in this repository](https://github.com/TheRealKaup/KTech/tree/master/ktech).

The core library is declared in the `ktech.hpp` header file of the `ktech` directory. This file is what you include in your game project. This is the "core" library since it is the minimum required to include in your project to start creating a game.

The contents of `ktech.hpp` are enclosed within a namespace called `KTech`.

The source files (`.cpp`) in the `ktech` directory are the core library's definitions.

The non-core parts of the library are currently only the widgets (UI objects) that are in the `ktech/widgets` directory. If you want to use any of the widgets there, you may selectively include the header files in there.

## Basic Structures

KTech comes with a couple of fundamental structures that are available via the `KTech` namespace. They are:
- `RGB` - Represents a single 24 bit depth color.
- `RGBA` - Represents a single 24 bit depth color with an extra alpha channel.
- `Point` - 2D vector comprising 2 32 bit signed integers (`x` and `y`).
- `UPoint` - 2D vector comprising 2 32 bit unsigned integers (`x` and `y`).
- `Cell` - Represents a terminal cell comprising of a foreground `RGB`, background `RGB` and a `char`.
- `CellA` - Represents a terminal cell with alpha channels comprising of a foreground `RGBA`, background `RGBA` and a `char`.

## World Hierarchy

The library contains more complex structures that build up a world. These structures are `Map`, `Layer`, `Camera`, `Object`, `Texture` and `Collider`, which are available via the `KTech` namespace.

```
          Map
         /   \
      Layer  Camera
       /      
    Object  
     /   \
Texture  Collider
```

### Map

Maps are completely disconnected from each other. They are a good way to divide your game to levels, for example. Maps contain a list of their layers.

### Layer

Layers are desigend to be loaded together. Layers contain a list of their objects.

When you render a map, it will render all of the layers in it, in the order they where added to the map. Meaning, textures in the first layer will be rendered first, and textures in the second layer will be rendered on top.

Physics-wise, objects interact only with other objects in the same layer.

### Object

Objects comprise textures and collider that behave relative to their parent object's position. Textures represent the object's appearance, and colliders represent the object's physical space.

### Texture

There are simple and complex textures, that both comprise of `CellA` values, beacuse textures are meant to be rendered for the terminal. Complex colliders are 2D arrays. Simple textures are instead rectangles, and thus are faster to render and have smaller memory usage, but they are of course less capable than complex colliders.

### Colliders

There are simple and complex colliders, in a similar manner to textures. Complex colliders are made of 2D array of `bool`s to represent where there is collider and where there isn't. Both complex and simple colliders have a `type` value, which is used to determine their collision result with other colliders.

Collision between objects can result in a push, a block, or an overlap event.

### Camera

Cameras are able to render a `Cell` based image that can be printed to the terminal, given layers that contain objects with textures.

## Engine

The `Engine` class comprises components that make the game run. These components manage different aspects, and are:
- `Collision` - Manages movement of objects and calculates collision events.
- `Audio` - Manages audio sources and playing audio
- `Time` - Manages invocations and provides game loop utilities
- `IO` - Manages user input and printing graphics
- `Memory` - Manages linking the world structure together

### `Collision`

Gets requests to move a certain object, calculates how its movement would look like, and based on that calls the relevant collision events on the relevant objects.

If you would look inside `ktech.hpp` at `Object` you would see a lot of virtual functions. These are the callback functions that `Collision` could call when it determines collision events.

You can create a class derived of `Object` and define new functionalities for these virtual functions to make them do things.

`Collision` also stores `colliderTypes` which represents the types of colliders that is global for the `Engine` instance. You can look at `colliderTypes` this way:

| What if (column) collides with (row) | Collider type 1 | Collider type 2 | Collider type 3 |
|--------------------------------------|-----------------|-----------------|-----------------|
| Collider type 1                      | Block           | Push            | Overlap         |
| Collider type 2                      | Block           | Push            | Overlap         |
| Collider type 3                      | Overlap         | Overlap         | Overlap         |

Meaning, type 1 is an unpushable collider, type 2 is a pushable collider, and type 3 is a collider that overlaps with everything.

### `Audio`

Currently `Audio` has no connections to the other components of the engine, and pretty much works as a wrapper to the "PortAudio" library.

`Audio` can load .wav files and create 1D audio sources, that can play, pause, resume and stop playing. It is pretty primitive.

### `Time`

`Time` provides utilities used to create a game loop (which will be discussed soon).

It can also create invocations that call given callback functions.

### `IO`

`IO` manages both input and output, although they are quite separate.

As for input, `IO` can be asked to attach a callback function when a certain key is pressed. It can also make groups of these attached callback functions, which make input handling easier, especially when it comes to UI.

As for output, `IO` can receive images rendered by `Camera`s to draw a "final image" that can later be actually printed to the terminal. The reason there is an extra step of drawing between rendering and printing is to allow multiple cameras at the same time to be displayed which can be useful for whatever reason.

### `Memory`

---
---
---

# OUTDATED - this document is very old to the point it is irrelevant, wait until it's updated.

**Tutorials:**
 - [Intro: KTech's interface, how to start using it](#intro)
 - [Loop](#loop)
 - [Basic Graphics](#basic-graphics)
 - [Input](#input)
 - [Collision](#collision)
 - [Audio](#audio)
 - [Widgets](#widgets)
 - Putting it all together
 - How KTech works, looking at the engine's files

**Notes:**
- KTech's documentation was written to be suitable for people who are new to game development.
- I **strongly advice** that whenever you encounter a programming concept which you are not familiar with, you should pause to learn it.
- I sometimes use the words "directory" and "folder" interchangeably.
- By saying "the engine" I mean the C++ game engine library, KTech.
- When I write "`filename`:", it means the following content is the file's entire content. And when I instead write "from `filename`:", it means the following content is a only a portion of the file's content. This is to save some text.

# Intro
## About this tutorial

In this tutorial, you will learn how to setup a KTech project, so you can start learning how to use the game engine.

## About using KTech

It is important to understand that KTech is only a C++ game engine library, which makes game development higher level. As of itself (meaning not including programs like TextureCreator), it doesn't have a GUI application that assists you in crafting your game.

To use KTech, you have to use the programming language C++, though you don't need to be all that experienced with the language.

If you know how to handle these concepts in C++, you should be more than fine:
- Classes
- Namespaces
- Functions (or "methods")
- Pointers
- Vectors
- Threads
- Compiling
- Header files

If you aren't familiar with something from the list or don't feel comfortable using it, you should pause to learn it.

## How to setup a KTech project

In the next section we will go over these steps in detail:

1. Download the "engine" folder from the KTech GitHub repository.
2. Put the engine folder in a directory which will be your game's code directory.
3. Create a C++ code file in your game's code directory, and call it whatever you'd like. For example, "game.cpp".
4. Include the KTech library in your game's code files by writing `#include <engine/engine.hpp>` at the start of the code.

## What did we just do

In the first step we downloaded the "engine" folder. The engine folder contains the KTech library. If you look inside, you will see some C++ files (.cpp) and C++ header files (.hpp).

In the second step we put the engine inside your game's code directory. You game's code directory doesn't have to be something specific, so create it wherever and name it however you'd like. This is just generally where you should place your game files (such assets and code), and the engine also needs to be there, or in a place where it is accessible from.

In the third step we created a C++ code file (like `game.cpp`) designated for your game's code. In this code file you will use the functions and classes from the engine folder to make your game. The engine files are tools which your game' code files use to make a game with logic and rules.

In the fourth step we included the engine's header file (`engine.hpp`) from the engine folder. This header file has all the engine's declerations of its functions, classes and variables. I recommend you to take a peek inside `engine.hpp` to see what it contains.

## The Engine namespace

All of the engine's stuff (classes, functions...) are stored in a namespace for clarity. The namespace's name is `Engine`, so to access something from it, write `Engine::Something`.

## Compiling

-needs to be written-

# Loop

## About this tutorial

In this tutorial, you will start learning how to use KTech by creating a game loop.

## The loop

A video game at its core is a loop which renders graphics, calculates physics, receives player inputs and processes them. This loop most commonly runs asynchronous, though can run synchronous with KTech. with the game play loop.

An iteration of the game loop is called a tick, and it runs at a certain ticks per second (TPS) rate, like, 12, 24, or 60 TPS. This is what gives a game the feel of a continuous world. For example, the very known game Minecraft by default runs at 20 TPS.

In KTech, the game develoepr creates their own game's loop. This allows for incredible control over your game, and a true understanding of what's happening and how everything works.

## Create a game loop

Firstly, let's setup a KTech project by following the [How to setup a KTech project](#how-to-setup-a-ktech-project) step-by-step guide.

Secondly, let's add a loop.

`game.cpp`:
```c++
#include <engine/engine.hpp>

int main()
{
	// Initialization
	while (Engine::running)
	{
		// Playing
	}
	// Exit
}
```

As you can see, this is just a simple while-loop, and you have complete control over it.

The `Initialization` part is where you initialize the game's world, like create a map, layers and objects.

The `Playing` part is where you run the game at the tick level. We will discuss what can be placed here in a moment

The `Exit` part is where you quit the game, for example, save the player's progress into a file.

`Engine::running` is a global boolean variable provided by the engine. I strongly recommend you use it because the engine will automatically set it to false when it detects an exit request from the terminal, meaning it will exit the loop and enter the exit part. This is more useful than having the whole thread taken down without notice and not reaching the exit part.

By running this program, nothing happens, this is obviously because our program is an infinite loop which does nothing. In fact, by ending the program, it will also not reach the exit part as expected by using `Engine::Running`, since we haven't yet initialized anything engine-related. Of course `Engine::running`'s functionality isn't working - our program is nothing but a plain C++ while-loop.

Let's continue by adding 2 things:
1. Initialization of `Engine::running`
2. Time handling

By time handling I mean keeping the loop run at a constant TPS.

Currently, our while-loop is blocking the thread by moving on to the next tick (iteration) without waiting. This is not good, because:
- We usually want our game to run at a consistent speed throughout the game play, and this is not consistent because it relies on the CPU's maximum speed and shifting availability.
- We also want our game's TPS to not be crazy high (relying on the maximum CPU perfomance can give us a TPS rate as high as a 5 digits number with a basic game), and instead something low like 144, 60 or 24 which is more comfortable to work with, and additionaly, creates a threshold which allows weak and strong systems to run the game at the same speed.
- Running this program on different systems with different maximum CPU speeds will result in varying game speeds, meaning it will change the game's whole behaviour.
- By blocking the CPU thread, we take over so much of the player's computing power, which is not very nice.

`game.cpp`:
```c++
#include <engine/engine.hpp>

int main()
{
	// Initialization
	Engine::PrepareTerminal({10, 10}); // Prepares terminal-related stuff
	Engine::tps = 24; // Set the TPS to 24
	Engine::thisTickStartTP.SetToNow(); // State the start time point of the first tick
	while (Engine::running)
	{
		// Playing
		Engine::WaitUntilNextTick(); // Suspend the thread until the next tick should start
	}
	Engine::ResetTerminal(); // Resets terminal settings that the engine set in PrepareTerminal()
	// Exit
}
```

- `Engine::PrepareTerminal({10, 10});` - This function sets terminal settings, such as hiding the cursor, and prepares other miscellaneous terminal-related stuff. The parameter is an `Engine::UPoint` (which is a class with 2 `unsigned long int` types, representing an unsigned 2D point) stating the size of the graphical ouput in character cells. This function is mandatory before printing anything to the terminal.
- `Engine::tps = 24;` - `Engine::tps` is a global variable that the engine provides. This, as the name suggets, states the ticks per second rate. This is mandatory before 
- `Engine::thisTickStartTP.SetToNow();` - Before entering the loop you need to inform the engine with the current time so `WaitUntilNextTick()` can correctly suspend the thread specifically after the first tick. This is what this line does. `Engine::thisTickStartTP` is an instance of class `Engine::TimePoint` which `WaitUntilNextTick()` uses to remember the last time it was called, and to know for how long to suspend the thread when called.
- `Engine::WaitUntilNextTick();` - will wait and return return until the time is right for the next tick, meaning it is suspending the thread and thus the loop. If there is no time left to wait for the next tick (perhaps because the game did too many operations this tick), then it will return immediately.
- `Engine::ResetTerminal()` - Resets terminal settings that the engine set in `Engine::PrepareTerminal()`, allowing for further use of the terminal after the game exits.

With these additions to the loop, our iterations run at a stable TPS rate.

You can always check what the functions do by simply checking their definitions in the engine folder. Look in the references to see where something is defined with a detailed explenation of what it does.

# Basic graphics

## About this tutorial

In this tutorial you will learn how to use graphics in KTech.

## How graphics work in KTech

KTech's graphics isn't like normal graphics. An image in KTech is made of character cells (since it is a terminal game engine), which are constructed of a character, the character's (foreground) RGB color and the background RGB color.

RGB in KTech is full 24 bit range (meaning 16777216 possible colors).

But this is just the the final rendered image which is displayd on the screen, because textures' character cells also have alpha channels (RGBA), allowing transparency and better graphics.

When textures overlap (have the same positions), the characters of the most upper texture will take place in the final image, so as the foreground color and background color, according to the alpha channels.

This could seem like a lot of information, but it's mostly techincal stuff which you won't be working with too often.

In order to display graphics on the terminal we need to do a couple of things:
1. [Have a world with visible objects.](#create-a-world)
2. Have a camera.
3. Prepare the terminal for printing.
4. Render with the camera.
5. Draw the camera's rendered image on the engine's final image.
6. Print the image.

In the next sections, we will go over the classes used while working with graphics in KTech, and then, in the following sections, we will construct a program that properly displays graphics, explainign everything in great detail.

## Coordinates

Coordinates in KTech work like how coordinates work in the field of printing and screens:

```
o----> X+
|
|
V
Y+
```

This is because of the order characters are printed into the terminal: characters start at the top left corner, characters go right as they are printed, and when they reach the end of the row they go down into a new line.

## Object

KTech uses a class called `Object`.

An `Object` has these things:
1. Position
2. Name
3. Vector of textures
4. Vector of colliders
5. On-event calls
6. Move() function
7. Some other stuff

Once again, you can check the references to see exactly what the `Object` class has with proper explenations, and you can also take a peek at the engine's code. But this list here is just to conceptualize `Object`.

So, `Object` is pretty much a physical, visible and functional, well, "object", which you put in your game's world.

Before we go into the adding an object into a world part, let's dive into what's a texture.

## Texture

The class `Object` uses the class `Texture` to store its textures.

The class `Textures` has:
1. Position (relative to the parent, or "owning", object)
2. Texture-creating functions `Rect()`, `File()` and `Write()`

And some other things which are listed in the references.

This what each texture-creating function does:
- `Rect()` - Draw a rectangle on the texture with a specified size, character, foreground color and background color.
- `File()` - Load texture from a KTech texture file, which can be made with TextureCreator. This allows for complex textures, with distinct colors and characters in all character cells.
- `Draw()` - Draw a texture by writing it in the code editor, with a specified foreground color and background color.

## Map

A `Map` is what a game world called in KTech.

`Map` has:
- Vector of layers
- Vector of cameras
- Other stuff

A `Map` is simply a collection of `Layer`s and `Camera`s.

## Layer

A `Layer` is a collection of objects.

It has a vector of objects and some other stuff.

Both `Map` and `Layer` are not very functional, and act as a group of things.

The reason `Camera`s are stored in a `Map` and `Object`s are stored in a `Layer` is that a camera can see the entire map, and the order of layers affects the order objects are rendered (objects in higher layers will be displayed on top of objects in lower layers). This allows for easier rendering order managment, and additionally, also affects collision, which we will cover in the following [collision tutorial](#collision).

## Camera

A `Camera` is a much more functional class.

It has:
- Position
- Resolution
- `Render()` function which transforms objects into an image and stores that image within itself.
- `Draw()` function which draws the stored rendered image on the engine's final image.

## The final image

"The engine's final image" is a global engine image which solves printing multiple images at the same time.

If for example you wish to display a rendered image at one half of the terminal and another rendered image at the other half, the final image allows this, because you can draw your images at a specified position and a rectangle (for cropping the image) on the final image before printing it. We will try this functionality later.

## Create a world

Let's create a new KTech project by simply adding another C++ code file in your game's code directory, and compile it instead of the C++ code file from the last tutorial. I will call this game "graphics_test.cpp" and copy the loop from the [game loop](#loop) tutorial to start things off.

`graphics_test.cpp`:
```c++
#include <engine/engine.hpp>

int main()
{
	// We will initialize our world here

	Engine::PrepareTerminal({10, 10});
	Engine::tps = 24;
	Engine::thisTickStartTP.SetToNow();
	while (Engine::running)
	{
		// And render and print our world here
		Engine::WaitUntilNextTick();
	}
	Engine::ResetTerminal();
}
```
It's important to note that this way of doing things is not mandatory. If your game doesn't require to render an image and print it every tick, you can change this behaviour to render an image and print it every time the player inputs a key, or a certain game event happens. This is one of the advantges of KTech being bare bone, it lets you run your game the way it fits your game. We will experiment with this in another tutorial.

Back to the topic. Notice the comments. We will create our world before entering the loop, and display our world inside the loop.

This makes sense: Before entering the playing state of the game we should initialize, or "load" it, and after everything was loaded we can enter the playing state by reaching the loop. In your own games you can of course create a different behaviour, for example, load only your game's main menu's assets, and when the player chooses to enter the actual game, unload the main menu's assets and load the game's assets and then enter the game.

But in this tutoril we will simply initialize our world and display it in the loop.

First, let's create a map:

from `graphics_test.cpp`:
```c++
int main()
{
	// We will initialize our world here
	Engine::Map map; // Create a map
	// ...Before PrepareTerminal
}
```

`Engine::Map` doesn't have a class constructor, which is common among KTech classes. `map` is now a completely empty map with no layers and no cameras.

Let's also create a layer and add it to the camera:

from `graphics_test.cpp`:
```c++
int main()
{
	// We will initialize our world here
	Engine::Map map;
	
	Engine::Layer layer; // Create a layer
	map.AddLayer(&layer); // Add the layer to the map
	// ...Before PrepareTerminal
}
```

Same as how we created `map`, `layer` has default values and no objects.

Using `Map::AddLayer` we add `layer` to `map` as a pointer. The usage of pointers here is important. This means that you are responsible for storing all of these class instances in the memory, and not the engine. This also means we can add objects and change attributes in `layer` after adding it to `map` and changes will apply, which is comfortable to work with and doesn't require a resource-heavy deep copy.

Now we have a map with an empty layer. Let's create an object and add it to the layer:

from `graphics_test.cpp`:
```c++
int main()
{
	// We will initialize our world here
	Engine::Map map;
	
	Engine::Layer layer;
	map.AddLayer(&layer);
	
	Engine::Object object({5, 5}); // Create an object
	layer.AddObject(&object); // Add the object to the layer
	// ...Before PrepareTerminal
}
```

`Engine::Object` has a constructor which all of its parameters are optional:
- Position of type `Engine::Point`
- Name of type `std::string`

Not specifying the position will set it to {0, 0}, and not specifying the name will set it to "" (empty).

We create an empty object with the position of {5, 5} and add it to layer.

Let's add a texture to the object:

from `graphics_test.cpp`:
```c++
int main()
{
	// We will initialize our world here
	Engine::Map map;
	
	Engine::Layer layer;
	map.AddLayer(&layer);
	
	Engine::Object object({5, 5});
	layer.AddObject(&object);	

	object.textures.resize(1); // Add a texture to the object
	object.textures[0].Write( // Draw the texture to be a pink character 
		{
			" O ",
			"/|\\",
			"/ \\"
		}, Engine::RGBA(255, 0, 255, 1.0f), Engine::RGBA(), Engine::Point( 0, 0 )
	);
	// ...Before PrepareTerminal
}
```

As you can see, neither the `Engine::Texture` class has a constructor, and instead, it has functions that draw the texture after it was created. This is comfortable with this writing scheme. By resizing the object's texture vector we create a new empty texture, and then we access it and call `Write` to draw this pink (255, 0, 255) character texture.

`Engine::RGBA` is used to store and pass RGBA values. The first `RGBA` parameter in `Write()` is the foreground color of the characters. The second `RGBA` is the background color, and because the default constructor of RGBA is (0, 0, 0, 0.0f), the background of this texture will be completely transperent. The `Point` parameter is the position of the texture relative to its parent object.

Once again: if you want to seek more information about `Write()` and the classes used here, simply check the references or look inside the engine's code.

And this it it, now we have a map with a layer with an object with a texture of a pink character.

## Print the world

In order to display the world we need a camera, so let's create one:


from `graphics_test.cpp`:
```c++
int main()
{
	// ...After creating the map
	Engine::Camera camera(Engine::Point(0, 0), Engine::UPoint(15, 10)); // Create a camera
	map.AddCamera(&camera, true); // Add the camera to the map
	// ...Before PrepareTerminal
}
```

`Engine::Camera`'s constructor has 2 parameters:
- Position of type `Point`
- Resolution of type `UPoint` (which is like Point but has 2 unsigned long integers instead)

In this example make a camera at the position of (0, 0) and with a resolution (in character cells) of (15, 10).

We then add `camera` to `map` and specify (the second parameter) to make it the active camera.

`Map::activeCameraI` is an index to the stated active camera from the cameras vector, which allows the nicer to use `Map::Render()` and `Map::Draw()` functions to work.

So now our `map` also has an active camera.

Next, we need to render an image, draw it to the engine's final image, and print the final image:

`graphics_test.cpp`:
```c++
int main()
{
	// ...After creating world
	Engine::PrepareTerminal({15, 10}); // We want the size of the final image to be (15, 10)
	Engine::tps = 24;s
	Engine::thisTickStartTP.SetToNow();
	while (Engine::running)
	{
		map.Render(); // Renders using the active camera
		map.Draw(); // Draws the active camera's rendered image
		Engine::Print(); // Print the final image into the terminal
		Engine::WaitUntilNextTick();
	}
}
```

And voila. Compile and run this, and you will see a pink character printed into the terminal 24 times a second. Admittedly, it's not impressive, but it's something. Here is the final piece of code:

`graphics_test.cpp`:
```c++
#include <engine/engine.hpp>
int main()
{
	// Initialize game world
	Engine::Map map;
	
	Engine::Camera camera(Engine::Point(0, 0), Engine::UPoint(15, 10)); // Create a camera
	map.AddCamera(&camera, true); // Add the camera to the map

	Engine::Layer layer;
	map.AddLayer(&layer);
	
	Engine::Object object({5, 5});
	layer.AddObject(&object);	

	object.textures.resize(1);
	object.textures[0].Write(
		{
			" O ",
			"/|\\",
			"/ \\"
		}, Engine::RGBA(255, 0, 255, 1.0f), Engine::RGBA(), Engine::Point( 0, 0 )
	);

	// Enter game loop
	Engine::PrepareTerminal({15, 10});
	Engine::tps = 24;
	Engine::thisTickStartTP.SetToNow();
	while (Engine::running)
	{
		map.Render();
		map.Draw();
		Engine::Print();
		Engine::WaitUntilNextTick();
	}
	// Exit
	Engine::ResetTerminal();
}
```

# Input

## About this tutorial

In this section we will make our blue square move, but first, we need to understand how input works with KTech.

## How input works in KTech

To demonstrate how it works, open a terminal application which allows you to type (such as a text editor or a shell), and hold one of the character keys. One character will be inserted, and after a short delay, many of that character will be inserted repeatedly until you release the key.

This is how KTech receives and distributes input, because KTech is a terminal game engine. For some certain game designs this is inconvenient, but it's a source for odd and unique solutions. There is no way to get a key state like in Unity or Unreal Engine, because terminal applications don't get access to this data. Terminal applications only get standard terminal input, which is adaquate for video games, and especially for UI based video games.

These are KTech's limitations:
- Keys start repeating if they are held for a certain length of time
- Only down events
- Only keyboard
- Keys states are unknown

Additionally, some inputs (like some modified keys) are received as escape codes, so check [the escape codes list]() from the references. There are some more important notes in that part of the references, so I strongly recommend you check that out.

## Get input

These are the steps:
- Call `PrepareTerminal()`
- Start an input thread
- Register input handler
- At the end of the program, call `ResetTerminal()`

We already covered `Engine::PrepareTerminal()` and `Engine::ResetTerminal()` before. You can always check the references to check out exactly what these engine functions do.

In order to receive input from the engine, we need to register an "input handler", which calls its assigned callback function when its key gets pressed.

Input related things are organized inside the `Input` namespace inside the `Engine` namespace.

## Register input handler

Inside the `Input` namespace there is the `RegisterHandler()` function. This function does what its name says - it puts in the engine's memory your callback function, and whenever the input loop receives the specified input the input loop will call that callback function.

from [the Engine::Input references]():

```c++
Engine::Input::RegisterHandler(
	std::string stringKey/char charKey,
	std::function<void()> call,
	bool onTick = false
)
```

- `stringKey`/`charKey` - the key, can be a character (such as 'a' or ' ') or a string (such as "\033[A").
- `call` a pointer to your callback function.
- `onTick` - (optional, `false` by default) we will cover exactly what this means, how to use it, and why it might be prefered in some cases later.

Let's register an input handler in the [previous program](#print-the-world):

from `game.cpp`
```c++
int ChangeObjectColor(&Engine::Object obj) // Our custom callback that changes an object's color 
{
	// Randomize the foreground color of the first texture
	obj.texture[0].SetForeground(Engine::RGBA(rand() % 256, rand() % 256, rand() % 256, 1.0f));
}

int main()
{
	/// ...After creating object
	srand(time(NULL)); // Generate a random see for later use
	Engine::Input::RegisterHandler('c', std::bind(ChangeObjectColor, &object)); // Register an input handler
	/// ...Before entering loop
}
```

This addition to the program will randomize our object's first texture foreground color (the character color) each time the 'c' key is received.

- `ChangeObjectColor()` is a new function we created. It calls `Engine::Texture::SetForeground()` for the given object which sets all the texture's character cells' foreground value.
- `srand(time(NULL))` generates random seed so `rand` in `ChangeObjectColor` will work.
- In `RegisterHandler`, we take `ChangeObjectColor()` and bind its `Object*` parameter with `&object` (a pointer of our object from before) using `std::bind`. This gives us a callback function with no parameters and returns, which is what `RegisterHandler` expects.

Though this won't work because the program is missing an input thread.

## Create an input thread

The input thread is another loop running asynchronous with the game loop.

The `Input` namespace has a premade input loop that can be ran as a separate thread:

from `engine/input.cpp`:
```c++
void Engine::Input::Loop()
{
	while (Engine::running)
		Get();
}
```

This is `Engine::Input::Loop()`'s definition, and as you can see, it is nothing more than a while loop that is calling `Get()` each iteration and will stop running if `Engine::running` is false.

`Engine::Input::Get()` is a blocking function which returns only after an input is received. This fact makes it unsuitable for the game loop if we wish to run the game loop at a consistent TPS, which is why in this example we will run the input loop in a separate thread, so the game loop can run at a consistent TPS and the input loop will get us player inputs.

What you need to know about this `Engine::Input::Loop()` is that if it is running, it will call your callbacks within your registered input handlers, which is the component we were missing so far.

Let's return to our program:

from `game.cpp`
```c++
int main()
{
	/// ...Preferably after initialization
	std::thread t_inputLoop(Engine::Input::Loop);
	/// ...Preferably right before entering game loop
}
```

Here, we create a new `std::thread` with a reference to the `Engine::Input::Loop` function.

The reason I advice you to place this right before entering the loop is because from that point the program will receive inputs which doesn't make much sense to receive when loading the game. It doesn't really matter though, and perhaps your game needs to start receiving input on startup, like if you want to integrate a cancel loading feature to exit the game. But, in this example we don't need such functionality, so I think it's most fitting that input will start coming only while actually playing.

Now, if you compile and run the program, it should randomize the foreground color of your object each time you hit the 'c' key.

## OnTick inputs

In the [register input handler section](#register-input-handler) we registered an input handler but we didn't expanded on what's the third parameter in `Engine::Input::RegisterHandler()`, `bool onTick`, is.

Setting `onTick` to false (the default value of the parameter) will make `Engine::Input::Get()`, which is called in `Engine::Input::Loop()`, to call callback functions when the specified input is received immediately, which is the basic functionality.

Setting `onTick` to true will make `Get()` collect all the times the input was pressed and distribute the presses as a single press each game loop tick.

In other words, setting `onTick` to true is deciding to synchronize the call of your callback function with the game loop, and avoiding calling callback functions multiple times each tick.

This is because the repeating rate of when you hold a key for a certain length of time is most likely not synchronized and faster than the game's TPS. This can lead to unwanted results, for example:

- Your character's movement keys start repeating at a rate faster than the game's TPS, and if the game is set up in a way that whenever the movement keys are pressed the character moves a cell, the character will skip a some cells every frame and then.
- You have a UI element in your game which allows the player to type text, and then the player holds a letter key which start repeating. Because the repeating rate is faster, in some frames it seems like 2 characters entered the text field at the same time.

You would probably prefer, in a game which intergrates a consistent TPS game loop, to receive the same key only once per tick, even if the repeating rate actually sent more than 1 presses of the key this tick.

In order to make `onTick` work, you need to call `Engine::Input::Call()` each tick since `Engine::Input::Loop()` simply can't just tell when a tick begins and ends. `Engine::Input::Call()` is desgisned to be placed inside a game loop: it distributes all of the collected inputs since the last time `Engine::Input::Call()` was called. 

Let's add it:

from `game.cpp`:
```c++
int main()
{
	// ...
	while (true) // The game loop
	{
		// Somewhere in the game loop, perhaps at the start
		Engine::Input::Call();
	}
	// ...
}
```

And now you can set `onTick` to true inside `Engine::Input::RegisterHandler`, and it shoud work as expected.

Really, this is mostly relevant in games which run at a low TPS (like 24), acknowledging that the repeating rate is usually not very high (like 30, though it isn't the same on every computer). This is because `onTick` doesn't solve keys which repeat at a lower (not a faster) rate than the game's TPS rate. It simply can't do that, since, as mentioned before, the engine doesn't get the necessary information, key states, for this.

## Moving character

I recommend that for making a game "character", one that is built of an object and custom logic and rules, you should create a unique class.

Let's start a new game, by creating a new game file which we will compile instead. In the context of this tutorial, rename the last game's code file ("game.cpp") to "game.cpp.old" and create a new game's code file named "game.cpp", so we have an old code file which we won't use anymore ("game.cpp.old") and a new, empty game code file ("game.cpp").

Let's start by creating a character class, but let's not actually call it "character", because the word "character" can be confused between the ubiquitous computer symbols and a person in a story or a game. So instead we will call it "Player".

### TODO: Mention about `Player` being struct and not class 

`game.cpp`:
```c++
#include <engine/engine.hpp>

struct Player // Our "Player" class
{
	Engine::Object object; // Has an object
	Player(Engine::Layer* layer) // Asks for a layer
	{
		// ...Create the object, initialize Player logic...
		layer->AddObject(&object); // Adds the object to the layer
	}
};

int main()
{
	// Initialize world
	Engine::Map map;
	
	Engine::Camera camera(Engine::Point(0, 0), Engine::UPoint(30, 15));
	map.AddCamera(&camera, true);

	Engine::Layer layer;
	map.AddLayer(&layer);
	
	Player player(&layer); // Create 

	// Enter game loop
	Engine::PrepareTerminal({30, 15});
	Engine::tps = 24;
	Engine::thisTickStartTP.SetToNow();
	std::thread t_inputLoop(Engine::Input::Loop);
	while (Engine::running)
	{
		Engine::Input::Call();
		map.Render();
		map.Draw();
		Engine::Print();
		Engine::WaitUntilNextTick();
	}

	// Exit
	Engine::ResetTerminal();
}

```

I borrowd the basic and useful program template from the previous couple of tutorials. It simply creates a map with a layer and a camera, prepares the terminal, enters a game loop with a consistent TPS, renders and prints graphics, calls collected inputs, and resets the terminal when exiting the game loop.

Usually in these tutorials, I'll comment after new lines. Here, the new things are the Player class and the creation of a player instance inside `main()`.

I suspect that the way `Player` is written is the way you would commonly write classes with KTech:
- Has at least one object.
- Has a constructor which requests a layer.
- Initializes its contents.
- Adds its one or more objects to the layer.

So our `Player` class has an object, a constructor, and inside the constructor adds its object to a given layer.

But we clearly didn't add anything more to the `Player` class, so the object does nothing.

Let's continue by making the object look like the player-looking object from previous tutorials:

from `game.cpp`
```c++
class Player
{
	Engine::Object object;
	Player(Engine::Layer* layer)
	{
		object.textures.resize(1);
		object.textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, Engine::RGBA(255, 0, 255, 1.0f), Engine::RGBA(), Engine::Point( 0, 0 )
		);

		layer->AddObject(&object);
	}
};
```

Now the object within `Player` has a pink person-looking texture.
I suggest we add 2 more constructor customizations, just for fun:
- Position
- Color

from `game.cpp`
```c++
class Player
{
	Engine::Object object;
	Player(Engine::Layer* layer, Engine::Point position, Engine::RGBA color)
	{
		object.pos = position;
		object.textures.resize(1);
		object.textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, color, Engine::RGBA(), Engine::Point( 0, 0 )
		);
		layer->AddObject(&object);
	}
};
```

So now the constructor also gets a position of type `Point` and a color of type `RGBA` and sets the player these values accordingly. We also need to update our `Player` instance creation in `main()`:

from `game.cpp`
```c++
int main()
{
	// ...Player creation, after creating layer
	Player player(&layer, Engine::Point(3, 3), Engine::RGBA(150, 150, 255, 1.0f)); // Create 
	// ...Before entering game loop
}
```

And now our player spawns at the position (3, 3) with a blue color.

Let's continue by adding movement for `Player`:

from `game.cpp`
```c++
class Player
{
	Engine::Object object;
	
	void Move()
	{
		if (Engine::Input::Is('w'))
			object.pos.y--;
		else if (Engine::Input::Is('a'))
			object.pos.x--;
		else if (Engine::Input::Is('s'))
			object.pos.y++;
		else if (Engine::Input::Is('d'))
			object.pos.x++;
	}
	
	Player(Engine::Layer* layer, Engine::Point position, Engine::RGBA color)
	{
		Engine::Input::RegisterHandler('w', std::bind(&Player::Move, this), true);
		Engine::Input::RegisterHandler('a', std::bind(&Player::Move, this), true);
		Engine::Input::RegisterHandler('s', std::bind(&Player::Move, this), true);
		Engine::Input::RegisterHandler('d', std::bind(&Player::Move, this), true);

		object.pos = position;
		object.textures.resize(1);
		object.textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, color, Engine::RGBA(), Engine::Point( 0, 0 )
		);
		layer->AddObject(&object);
	}
};
```

`Engine::Input::Is()` is a function to check if the last input is equal to the parameter. You can also directly access the input buffer (`Engine::Input::buf`) though I see no reason to, and it's easier to write with `Is()`.

We add and subtract 1 cell according to the last input. Because up is -1 on the Y axis then we are subtracting 1 from the Y value of the object's position when the 'w' key is pressed.

The usage of `std::bind()` in `RegisterHandler()` inside the constructor makes it possible for the engine to call the `Move()` of this class instance.

And this should be it. Also notice that we are setting the `onTick` parameter in `RegsiterHandler` to true since it simply fits our consistent and low TPS game loop, and that we are calling `Engine::Input::Call()` in the game loop to make this work.

Compile and run this program, and really, we can call this a game by this point, since we have a visible character that moves according to the player's input.

# Collision

## About this tutorial

In this tutorial you will learn how collision works and how to use it. We will expand the program from the last tutorial and add objects with colliders to the world and make them behave in different ways.

## Collision in KTech

The collision subsystem in KTech is made of a couple of important elements which we will cover in detail in the following sections:
- [`Layer`s with `Object`s with `Collider`s](#world-with-physical-space)
- Collider types
- `Object::Move()`
- "on-event"s

Collision in KTech is done with a single operation called whenever an object wants to move. This is in contrast to a physics engine which works in a continuous way, which updates positions of objects each tick according to their velocity et cetera.

## World with physical space

As we already touched upon, layers are a way to manage objects within maps. Graphics-wise, the layer's index is equal the rendering order of objects in a map.

Collision-wise, objects interact only with other objects in the same layer. You can look at layers as a third Z axis: objects from different layers can't collide with each other because they are in different Z positions, but a camera can still render objects from different layers as long as they are in the X and Y range, and the Z axis determines the rendering order.

This is the first time we encounter the `Engine::Collider` class. It behaves in a pretty similar manner to `Engine::Texture`, both:
- Are stored in vectors inside `Object`'s.
- Have a simple form and a complex form
- Have variables such as positions relative to the parent object and active status
- Have different constructors

The current version of the engine only supports what's called a simple collider. A simple collider is a rectangle, which is stored inside 2 `Point` class instances; a size and a position relative to the parent object. A compelx collider, which is currently not working, is supposed to be a 2D boolean vector representing the collider, allowing you to create complex shaped colliders. But once again, these don't work at the moment, and will probably return in the near future.

So, this simple collider rectangle represents the physical spaces of objects.

`Engine::Collider` has 2 constructors, one for the complex collider and the second for the simple, so at the moment, only use the simple one. Check the references for the exact `Engine::Collider`'s content and constructors.

`Object`s have a `Collider` vector called `Object::colliders`. Meaning, a single `Object` can have multiple `Collider`s. This is useful for many purposes. For exmaple, let's say you are creating a platform game which allows the player to jump. To know if the player can jump, you should check if the player is on the ground and not mid-air, and a way to to this is to add an additional collider (beside the normal body collider) under their legs which allows you tell if there is ground beneath the player.

## Collider types

Each `Collider` has a collider type which is used to determine the collision result between 2 colliders.

There are 3 possible collision results:
1. Block (result=0), the moving collider can't move because of the other collider.
2. Push (result=1), the moving collider can move, and will also move the other collider with it to clear the way. 
3. Overlap (result=2), the moving collider can move into the other collider.

The collider types are stored in the 2D unsigned char (8 bit number) vector `Engine::colliderTypes` which you can define to fit your needs. These are the default collider types:
```c++
std::vector<std::vector<unsigned char>> Engine::colliderTypes = {
	{ 0, 1, 2 }, // Heavy - 0
	{ 0, 1, 2 }, // Normal - 1
	{ 2, 2, 2 } // Overlappable - 2
};
```

This is definitely confusing to look at for the first time. This table states all of the collision results between colliders; every row is a collider type which is moving, and every column is a collider type which is being moved into. This is how you should be looking at it:

| What happens when type A collider collides with type B collider? | B=0         | B=1         | B=2         |
|------------------------------------------------------------------|-------------|-------------|-------------|
| **A=0**                                                          | 0 (Block)   | 1 (Push)    | 2 (Overlap) |
| **A=1**                                                          | 0 (Block)   | 1 (Push)    | 2 (Overlap) |
| **A=2**                                                          | 2 (Overlap) | 2 (Overlap) | 2 (Overlap) |

Meaning, heavy colliders can't be pushed, normal colliders can be pushed, and overlappable colliders overlap and can be overlapped.

For example, if a type 0 collider collides with a type 1 collider, the moving object will push the other object.

If we would want, for whatever reason, that this collision result will instead be, for example, block, we would change that value in the collider types table by writing `Engine::colliderTypes[0][1] = 0;` or `Engine::colliderTypes = {{ 0, 0, 2 }, { 0, 1, 2 }, { 2, 2, 2 }};`.

The `Engine::Collider` class has a `Collider::colliderType` integer variable. This variable represetns the collider type of the collider. As covered earlier, a single `Object` can have multiple `Collider`s, which means an `Object` can have multiple `Collider`s with of different types, which allows advanced behaviour.

The `Collider::colliderType` variable's value needs to be set between the `Engine::colliderTypes` rows index range, otherwise, it will have a undefined collider type.

## `Object::Move()`

In practice, calculating collision in KTech is done by calling the `Object::Move()` function which processes the colliders in the space, determiens a move result, changes positions of objects and ends by calling "on-event"s. This means that 

This function is what makes things happen.

It takes a single `Point` parameter for the wanted movement direction.

`Object::Move()`'s goal is to check if movement can be done, by processing the object's environment (inside `Object::parentLayer`), object by object, collider by collider. `Object::Move()` always concludes a successful movement which will lead to changing `Object::pos` accordingly, or a blocked movement which will lead to nothing happening. These 2 conclusions can mean:
- Succesful movement: there is nothing is in the way, or, there is an object in the way and the moving object managed to push it, or, there is an object in the way and the moving object can go though it (overlap).
- Blocked movement: there is an object in the way and the moving object can't push it.

`Object::Move()` also can do what I call "tree movement". This allows for an object to push an object which will push another object, kind of a movement sequence, or a "tree". This means that if an object is moving and there is a blocking object in the way, it will first check if there are other objects which it can push, and will check if those pushable objects can push that blocking object. Tree movement is always in favor of moving and not being blocked, meaning, it will not stop searching for a way to push blocking objects until there are no more relevant object to search for.

In short, by actually calling `Object::Move()` with a given direction, `Move()` tries to change the position of the moving object with the other objects from the moving object's parent layer in mind.

## On event callback functions

Additionally, `Object::Move()` calls "on event" callback functions. Check these variables inside `Object`:

```c++
Object* theOtherObject = NULL;
Point lastPush = { 0, 0 };
int theColliderIndex = -1;
int theOtherColliderIndex = -1;
std::function<void()> OnPushed = NULL;
std::function<void()> OnPush = NULL;
std::function<void()> OnBlocked = NULL;
std::function<void()> OnBlock = NULL;
```

- `OnPushed` is a pointer you set to a function you define (a callback function) which gets called when this object is being pushed.
- `OnPush` is a callback function which gets called when this object pushes another objet. 
- `OnBlocked` is a callback function which gets called when this object gets blocked (if it just moved or it was attempted to be pushed into an eventually blocking object).
- `OnBlock` is a callback function whch gets called when this object blocks another object.
- `lastPush` is a `Point` specifying the direction of the last time it was pushed. It is set before whenever `OnPushed` and `OnPush` are called.
- `theOtherObject` is a pointer to an object which is set to the relevant object whenever `OnPushed`, `OnPush`, `OnBlocked` and `OnBlock` are called.
- `theColliderIndex` is an integer which is set to the relevant index in this `Object`'s `colliders` vector whenever `OnPushed`, `OnPush`, `OnBlocked` and `OnBlock` are called.
- `theOtherColliderIndex` is an integer which is set to the relevant index in the `theOtherObject`'s `colliders` vector whenever `OnPushed`, `OnPush`, `OnBlocked` and `OnBlock` are called.

These callback functions and variables are useful for adding rules and logic into your game after calling `Object::Move()`.

For example, if I had a custom class with an object and I wanted that whenever it gets touched, the touching object will disappear, I would set `OnBlock` and `OnPushed` to a function which uses the `theOtherObject` object pointer to remove the object from the layer:

```c++
struct ObjectWhichMakesThingsDiappear
{
	Engine::Object object;

	void RemoveOtherObject()
	{
		object.parentLayer->RemoveObject(object.theOtherObject);
	}

	ObjectWhichMakesThingsDiappear(Engine::Layer* layer)
	{
		// ...Add a collider to the object et cetera (we didn't cover how to actually do this yet)
		object.OnBlock = std::bind(&ObjectWhichMakesThingsDiappear::RemoveOtherObject, this);
		object.OnPushed = std::bind(&ObjectWhichMakesThingsDiappear::RemoveOtherObject, this);
		layer->AddObject(&object);
	}
};
```

## Game plan

In the following sections we will cover collision in practice by making a game which:
- Defines its own collider types
- Creates a world with multiple objects, some with multiple colliders and some with different collider types
- Calls `Object::Move()` with player input
- Harneses the power of on event callback functions

Unlike the previous programs in the last tutorials, you can really call this program a game; it will be a much larger program with real gameplay.

Our game will be a simple obstacle course with multiple levels. The player's mission is to get from the starting point to the ending point without hitting the moving enemey objects. There will also be a mechanic to open doors by dragging boxes onto pressure plates.

The first level will look like this:
```
#########
#   E   #
#       #
#X<---> #
#       #
# <--->X#
#       #
#X<---> #
#       #
#   S   #
#########
```

- `S` - Starting point
- `E` - Ending point
- `#` - Wall, blocking border
- `X` - Enemy, the player fails if they touch it
- `<->` - Enemy movement path

The player starts at the bottom and their goal is to get to the top. The enemies move back and forth on their path. We will use the movement from the [input tutorial](), so we need to have the way input works in mind.

The second level:
```
#########
#E#  B  #
# #     #
# # <->X#
# #     #
# #     #
# #X<-> #
# #     #
# D  S P#
#########
```

- `D` - Door
- `P` - Pressure plate
- `B` - Draggable box

The player's mission in this level is to avoid the moving enemies through the corridor in order to reach the box and drag it back onto the pressure plate at the bottom right, which will open the door at the bottom left, letting the player get to the ending point.

To make this even more challenging, when the player holds a box, the box will become a part of them so if an enemy hits the box that will also fail the player.

The third and last level:
``` 
################
#P<---\D/-->--\#
#####//#|#####|#
# S #|##|#/X\#|#
#   #|#PX#|E|#X#
#    |#B|#\</#|#
# B  v##|##D##|#
#    X##\--<--/#
################
```

The player's mission is to drag the box into the first enemy's movement path, which will make the enemy push the box onto the pressure plate. The player needs to let the enemy drag it since if they try to drag the box themselves, the enemy might to reach them before they manage to enter the door. Behind the door is another room with 2 enemies circling a smaller room, the player needs to follow the enemies and keep up the speed so they don't get hit or hit an enemy. In the left side of the room there is another set of box that needs to be placed upon a pressure plate which will open the small room's door. To make this easier to understand in game, we will color buttons and doors the same. Inside the small room there is a nother enemy which circles around the ending point.

After the player finished the first level, the game will start the second level, after that the third, and after that will finish the game and exit it.

## First level

For now we will use the default collider types, which are heavy, normal and overlappable.

Our walls will have heavy colliders, meaning they can't be pushed and are fixed into placed. Normal and heavy colliders will be blocked if they try to move into heavy colliders.

Let's start with an empty game loop, input thread, a map with a camera and a layer, 16 * 11 image size (which should be about what we need), a moving player class, 

`game.cpp`:
```c++
#include <engine/engine.hpp>

class Player
{
	Engine::Object object;
	
	void Move()
	{
		if (Engine::Input::Is('w'))
			object.pos.y--;
		else if (Engine::Input::Is('a'))
			object.pos.x--;
		else if (Engine::Input::Is('s'))
			object.pos.y++;
		else if (Engine::Input::Is('d'))
			object.pos.x++;
	}
	
	Player(Engine::Layer* layer, Engine::Point position, Engine::RGBA color)
	{
		Engine::Input::RegisterHandler('w', std::bind(&Player::Move, this), true);
		Engine::Input::RegisterHandler('a', std::bind(&Player::Move, this), true);
		Engine::Input::RegisterHandler('s', std::bind(&Player::Move, this), true);
		Engine::Input::RegisterHandler('d', std::bind(&Player::Move, this), true);

		object.pos = position;
		object.textures.resize(1);
		object.textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, color, Engine::RGBA(), Engine::Point( 0, 0 )
		);
		layer->AddObject(&object);
	}
};

int main()
{
	// Initialize world
	Engine::Map map;
	
	Engine::Camera camera(Engine::Point(0, 0), Engine::UPoint(16, 11));
	map.AddCamera(&camera, true);

	Engine::Layer layer;
	map.AddLayer(&layer);
	
	// Enter game loop
	Engine::PrepareTerminal({16, 11});
	std::thread t_inputLoop(Engine::Input::Loop);
	Engine::tps = 24;
	Engine::thisTickStartTP.SetToNow();
	while (Engine::running)
	{
		Engine::Input::Call();
		map.Render();
		map.Draw();
		Engine::Print();
		Engine::WaitUntilNextTick();
	}

	// Exit
	Engine::ResetTerminal();
}
```
