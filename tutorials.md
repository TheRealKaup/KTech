**Tutorials:**
  - Intro: KTech's interface, how to start using it
  - Making a simple platform game
  - How KTech works, looking at the engine's files

**Notes:**

- KTech's documentation was written to be suitable for people who are new to game development, and even to some computer programming concepts.
- I **strongly advice** that whenever you encounter a programming concept which you are not familiar with, you should pause to learn it.
- I sometimes use the words "directory" and "folder" interchangeably, as well as "KTech" and "the engine".
- When I write "`filename`:", it means the following content is the file's entire content. And when I instead write "from `filename`:", it means the following content is a only a portion of the file's content. This is to save some text.

# Intro: KTech's interface, how to start using it

### About this tutorial

In this tutorial, you will learn how to setup a KTech project, so you can start learning how to use the game engine.

### A little about using KTech

KTech, unlike popular game engines such as *Unreal Engine* and *Unity*, isn't as of itself a GUI application. It is only a C++ library which you put in your game engine directory and use it as you wish.

A "KTech game project" in reality is your project which also uses KTech. What I am trying to say is that unlike, for example *Unreal Engine*, KTech isn't a super-rich and powerful software development kit (SDK) application which creates for you the game files, runs your game, and holds everything together.

With KTech, you do all of these things.

KTech is a very bare bone game engine, in which things, that in other game engines are abstracted to a drag & drop exeperience, are done by writing code.

For example, to place the player character in the game world, in...
- *Unreal Engine* you would drag the character from the game's directory to the game editor viewport.
- KTech you would set the character's position in its object's class constructor and call a function which adds the object to the game world.

The reason these are such different experiences is because of the level they operate at. A drag & drop experience, when you hit run to play the game, goes down a level and gets converted into an operation which gives the character a position and adds it to the game world, which sounds familiar.

This information isn't too important for you if you didn't come with a background of using game engines that have these drag & drop experiences and other abstractions which hide what truly happen. The reason I am emphesizing this major difference is because when I stopped using Unity and Unreal Engine in order to make this game engine, understanding this point really assisted me to get a solid grasp of what I was doing.

### Setup a KTech project

1. Download the "engine" folder from the KTech GitHub repository.
2. Put the engine folder in a directory which will be your game's code directory.
3. Create a C++ code file in your game's code directory, and call it whatever you'd like. For example, "game.cpp".
4. Include the KTech library in your game's code files by writing `#include <engine/engine.hpp>` at the start of the code.

### What did we just do

The "engine" folder is the KTech game engine's code, and it is important to understand that KTech is nothing more than that. KTech isn't a software you need to launch in order to create a world with logic and rules. KTech is a collection of tools, expressed as code, that make game development easier and more accessible, or in other words, a library.

By writing `#include <other_code_file_name.hpp>` in a code file, you basically get access to its content. I enourage you to check the content of the file "engine.hpp" in the engine directory. You don't have to understand what everything there means, but notice the things such as `Map`, `AudioSource`, or `InitializeAudio`. These are all the concepts, tools or however you'd like to call them, which the engine provides. The other code files in the folder define the functionality of these functions.

So, by adding `#include <engine/engine.hpp>` at the start of your game's code files, you are accessing the concepts and functions in that C++ header file (.hpp), which are defined in the other C++ code files (.cpp).

### About KTech classes

Concepts, such as a map, an object and a camera are classes.

In the making of your game, you create instances of these classes.

It is important to understand that unlike other common game engines, you are responsible for creating, keeping and changing instances of these classes, and not, for example,  a physics system that automatically updates your objects position according to gravity, or, a drag and drop experience to set the positions of cameras and objects at the start of the game. And also, you can always go deeper and remove or replace KTech functionality with your own, in case you feel like you still don't have enough control.

It might sound like game development with KTech is very manual and tedious, but fear not, because the engine provides functions that make it easy to program your game's functionality, which saves you writing compelx and long code, but doesn't save you from pretty much infinite control over your game. This is a considerable advantage over common game engines, which either hide their code and functionality, or are too big and complicated to understand, analyze, and modify.

### How to actually access classes and engine functions

If you look inside `engine.hpp`, you will see that everything is written within a "namespace". A namespace is used to organize code, and make things clearer especially when you use the namespace's content.

In order to reach an engine class, for example, the "Map" class, you have to access it from the namespace, by specifying the namespace.

Let's create a map:

`game.cpp`:

```c++
#include <engine/engine.hpp>

int main()
{
	Engine::Map map;
}
```

Here, the game starts from the usual C++ entry point, `main()`. We use the `Map` class from the `Engine` namespace we got access to by including in the first line the "engine.hpp" header file from the "engine" folder we downloaded into our game development directory.

This program, or "game", by the way, creates an empty map an immediately exists and ends.

### Creating a world

If you come from other game engines, like Unity or Unreal Engine, the code from the last section is pretty much what these engines do at the start of a game if you told them in the game making editor that there is a map.

This is the core difference between the game development experiences, the fact that there is no "editor" for this kind of stuff in KTech.

*This, though, might actually change in the future. Just like how there is TextureCreator (an application made with KTech for creating KTech game textures) for making KTech game development easier, there can be more of these applications to create, for example, maps, or, animations.

Let's expand the program from before and also add a layer to the map, and an object to that layer, and a texture to that object.

A layer, by the way, allows to seperate objects in the same map if you wish so. For example, you can have a layer for the ground and a layer for clouds.

Step 1: create a layer, and add the layer to the map.

`game.cpp`:

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

`game.cpp`:

```c++
#include <engine/engine.hpp>

int main()
{
	Engine::Map map; // Create an empty map

	Engine::Layer layer; // Create an empty layer
	map.AddLayer(&layer); // Add the layer to the map

	Engine::Object object( // Create an object
		Engine::Vector2D(5, 5), // Set the position
		"My Object"); // Set the object's name
}
```

Here, we created an object and used its class constructor. `Object` has 2 optional parameters, meaning you don't have to fill them, and instead use the default values which will set the object's position to (0, 0) and the name to "" (empty). `Engine::Vector2D` is another class like `Map`, `Layer` and `Object`, which has 2 variables inside of it: an X value and a Y value. So, so far we have an object that is at the position of (5, 5) and is called "My Object".

Step 3: add a texture to the object, and add the object to the layer.

`game.cpp`:

```c++
#include <engine/engine.hpp>

int main()
{
	Engine::Map map; // Create an empty map

	Engine::Layer layer; // Create an empty layer
	map.AddLayer(&layer); // Add the layer to the map

	Engine::Object object( // Create an object
		Engine::Vector2D(5, 5), // Set the position
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

In the next tutorial, we'll make a game that will actually run, display our world and even play some sounds.

# Making a simple platform game

#### About this tutorial

In this tutorial, you will learn how to **use** KTech and make a game with it.

Don't worry if until this point of the documentation you have heard KTech concepts that you aren't familiar with, because this is the tutorial that is supposed to teach you these things about KTech.

#### The loop

The last piece of code in this documentation was a program, but not a game.

What makes a game, a game?

A game at its core is a loop. Some games are turn-based, some not, but almost always you can find a game play loop within it. For example: the player harvests resrouces, with those resources crafts a tool, with that tool gets more resources faster, with those resources crafts a better tool, and so on.

But, specifically a **video** game, has another kind of loop, working asynchronous with that other loop. That loop renders graphics, calculates physics, receives player inputs and processes them.

When making a game with, for example, Unity or Unreal Engine, you do get some glimpses of that base, true core, loop. Most commonly, by asking the game engine to call our functions ("user-defined functions") **on tick**. An iteration of that loop is called a **tick**, and it runs a certain amount of ticks per second, for example, 12, 24, or 60 ticks per second. For example, the very known game Minecraft by default runs at 20 ticks per second.

**In KTech, the game develoepr creates their own game's base loop.** This allows for incredible control over your game, and a true understanding of how everything works.

#### How to actually create a base loop

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

Yep, that's it. This is the loop, and it's completely yours.

The `Initialization` part is where you initialize the world, meaning, create the map, the layers, the objects.

The `Playing` part is where you run the game, at the base loop level. We will learn how to do these things later, but as an example, rendering an image is something that can be done in this loop.

The `Exit` part is where you quit the game, for example, save the player's progress into a file.

`running` is a global boolean variable provided by engine, as you can see from the `Engine::`. You can ditch it, but I recommend you use it, because the engine will automatically set it to false when it detects an exit request from the terminal, meaning it will exit the loops.

Even though we have a base loop, it's still hard to call this a game, because all this is doing is entering a blank infinite loop. In fact, not even the functionality I mentioned earlier about `Engine::running` will work if you run this, because this program doesn't initialize that engine functionality.

Let's combine the program from the last tutorial with this one:

`game.cpp`:

```c++
#include <engine/engine.hpp>

int main()
{
	// Initialization
	Engine::Map map; // Create an empty map

	Engine::Layer layer; // Create an empty layer
	map.AddLayer(&layer); // Add the layer to the map

	Engine::Object object( // Create an object
		Engine::Vector2D(5, 5), // Set the position
		"My Object"); // Set the object's name

	object.textures.resize(1); // Add space to the texture array
	object.textures[0].Rectangle( // Draw a rectangle shape
		Engine::UVector2D(3, 3), // The size of the rectangle
		Engine::SuperChar('#', Engine::RGBA(0, 0, 255, 1.0f)), // The value each character cell
		Engine::Vector2D(0, 0)); // The position of the texture relative to the object

	layer.AddObject(&object); // Add the object to the layer

	// Base loop
	while (Engine::running)
	{
		// Playing
	}

	// Exit
}
```

Now we are getting somewhere, though our program is still not a game. Let's once again analyze what this program does:

1. It creates an empty map.
2. It creates an empty layer.
3. It adds the layer to the map, so, at this point we have a map with an empty layer.
4. It creats an empty object.
5. It expands the object's textures vector (array) by 1, allowing us to draw a texture at the first index of the object's texture vector.
6. It sets the first texture of the object to a blue 3 by 3 square made of the '#' character.
7. It adds the object to the layer, so, at this point we have a map with a layer with an object that has a blue square texture.
8. At this point it finished initializing the game and now enters the infinite game base loop, which will end if the game gets forcfully quitted (for example by pressing Ctrl+C which is standard for exiting terminal applications).

Let's make the base loop display our world.

First, we are missing a camera, let's create one in the initialization part, somewhere after we create the map:

from `game.cpp`:

```c++
// ...after creating the map
Engine::Camera camera({ 0, 0 }, { 20, 20 });
map.AddCamera(&camera, true);
// ...before entering the base loop
```

The first parameter in the camera class constructor, which is a Vector2D, `{0, 0}`, is the position of the camera.

The second parameters (also a Vector2D) is the size of the camera's image, the "resolution", so here it is 20 by 20, which might sound small, but this size is not in pixels, it is in character cells. The player can resize their terminal's font if they want to make the game larger on their screen.

After making the camera, we add it to the map. The first parameter for the map's function "`AddCamera`" is a pointer the camera, and the second parameter asks us if we want this camera to be the active one. This is useful if your map has multiple cameras you wish to switch between. We don't add the camera to a layer, because layers separate objects only on the physical sepctrum (collision), but a camera renders all the objects in a map. Objects in higher layers will be displayd on top.

Before we tell the camera to render and print in the base loop, you must prepare the terminal for printing, before entering the base loop:

from `game.cpp`:

```c++
// ...after initializing world
Engine::PrepareTerminal({20, 20});
// ...before entering the base loop
```

`PrepareTerminal` prepares a global image, which is like a final image before printing a rendered image (allowing printing multiple images at the same time at different locations on the terminal). It also prepares some other stuff regarding the termianl. If you want to check out exactly what it does, you can simply look in the `engine.cpp` file in the engine folder, and look for this engine function definition. This, by the way, enables the `Engine:running` functionality discussed earlier.

You should also call the engine function `Engine::ResetTerminal()` at the exit part of the game, which will reset the termianl settings set by the engine, allowing for further use of the terminal after the game exits:

from `game.cpp`:

```c++
// ...after the base loop
Engine::ResetTerminal();
// ...before the program's end
```

Now, let's add the base loop functionality:

from `game.cpp`:

```c++
// ...after initialization
while (Engine::running) // The base loop
{
	map.Render();
	map.Draw();
	Engine::Print();
}
// ...before exit
```

`Map::Render()` renders an image with the map's active camera. It stores the image inside that active camera, awaiting to be drawn.

`Map::Draw()` draws the rendered image of the map's active camera to the final engine image.

The final engine image allows printing multiple images at the same time, at different positions.

Also, look at `Map::Draw()`'s optional parameters listed in `engine.hpp`. You can tell `Map::Draw()` to draw the image at a certain position on the final image, and specify a rectangle to crop the camera's image.

`Engine::Print()`, lastly, prints the engine's final image to the terminal, which actually displays the image.

Let's take a look at our current version of the program:

`game.cpp`:

```c++
#include <engine/engine.hpp>

int main()
{
	// Initialization
	Engine::Map map; // Create an empty map

	Engine::Layer layer; // Create an empty layer
	map.AddLayer(&layer); // Add the layer to the map

	Engine::Camera camera({ 0, 0 }, { 20, 20 }); // Create a camera
	map.AddCamera(&camera, true); // Add the camera to the map

	Engine::Object object( // Create an object
		Engine::Vector2D(5, 5), // Set the position
		"My Object"); // Set the object's name

	object.textures.resize(1); // Add space to the texture array
	object.textures[0].Rectangle( // Draw a rectangle shape
		Engine::UVector2D(3, 3), // The size of the rectangle
		Engine::SuperChar('#', Engine::RGBA(0, 0, 255, 1.0f)), // The value each character cell
		Engine::Vector2D(0, 0)); // The position of the texture relative to the object

	layer.AddObject(&object); // Add the object to the layer

	Engine::PrepareTerminal({20, 20});

	// Base loop
	while (Engine::running)
	{
		// Running
		map.Render();
		map.Draw();
		Engine::Print();
	}

	// Exit
	Engine::ResetTerminal();
}
```

I still don't think we can call this a game, but it is definitely a renderer. If you run it, it will display a blue square of hashes.

#### Timing

Here is a small practice, try to answer these questions about the current version of our program:

1. What are the 3 parts of the program?
2. What happens in the first part?
3. What happens in the second?
4. What happens in the third?
5. What is the ticks per second (TPS) rate (how many iterations the base loop goes through per second)?

The answer for 5 is as high as your computer allows, because our-while loop is blocking the thread by immediately executing the next operation eternally. This is not good, because:

- We usually want our game to run at a consistent speed throughout the game play, and this is not consistent because it relies on the CPU's maximum speed and shifting availability.
- We also want our game's TPS to not be crazy high (relying on the maximum CPU perfomance can give us a TPS rate as high as a 5 digits number), and instead something low like 144, 60 or 24 which are numbers that are more comfortable to work with.
- Running this program on different systems with different maximum CPU speeds will result in varying game speeds.
- By blocking the CPU thread, we take over so much of the player's computing power, which is not cool.

To clarify, by "speed" I don't just mean the game's graphical FPS, but the entire game's speed. If you design your game to move your character 1 character cell every tick, different TPSs will result in different character speed.

#### How to actually limit TPS

The solution is to limit our TPS, by suspending our thread with specific time values. The goal is to give our base loop a consistent and smooth TPS rate.

Additionaly, setting a low enough TPS (like 144, 60, 24, et cetera) creates a threshold which allows weak and strong systems to run the game at the same speed.

To do this, the engine provides a function and a global variable:

- `Engine::tps` - this global variables declares the game's TPS rate.
- `Engine::WaitUntilNextTick()` - this function suspends the thread until the next tick should start.

`WaitUntilNextTick()` knows how much time is left for the current tick based on the last time it was called, which makes it convenient to place in a loop.

Let's implement this TPS limiting functionality into our loop:

from `game.cpp`:

```c++
// ...after initialization
Engine::thisTickStartTP.SetToNow(); // State the start time point of the first tick
Engine::tps = 24; // Set the TPS to 24
// Base loop
while (Engine::running)
{
	map.Render();
	map.Draw();
	Engine::Print();
	Engine::WaitUntilNextTick(); // Suspend the thread until the next tick should start
}
// ...before exiting
```

- `Engine::thisTickStartTP.SetToNow();` - Before entering the loop you need to inform the engine with the current time so `WaitUntilNextTick()` can correctly suspend the thread specifically after the first tick. This is what this line does. `Engine::thisTickStartTP` is an instance of class `Engine::TimePoint` which `WaitUntilNextTick()` uses to remember the last time it was called, and to know for how long to suspend the thread.
- `Engine::tps = 24;` - sets the TPS before entering the loop, but you can change its value at any point and it should act correspondingly.
- `Engine::WaitUntilNextTick();` - will return until the time is right for the next tick, meaning it is "blocking" the loop. If there is no time left to wait for the next tick, perhaps because the game did too many operations this tick, then it will return immediately.

Our "game" now properly runs at a consistent TPS.

#### Input with KTech

In this section we will make our blue square move, but first, we need to understand how input works with KTech.

To demonstrate how it works, open an application where you can type text in, and hold one of the character keys. One character will be inserted, and after a short delay, many of that character will be inserted repeatedly until you release the key.

This is how KTech receives and distributes input, because KTech is a terminal game engine. For some certain game designs this is inconvenient, but it's a source for odd and unique solutions. There is no way to get a key state like in Unity or Unreal Engine, because terminal applications don't get access to such information. Terminal applications only get standard terminal input, but really, it's pretty fine for video games, especially for ones that are mostly UI based.

In order to receive input from the engine, we need to register an "input handler", which calls a callback function when the input handler's key gets pressed.

So, the input limitations are:

- No way to know key states
- Keys start repeating if they are held for a certain length of time
- No way to know when keys are released (only pressed
- Only keyboard, no mouse input

#### How to actually get input

Inside the `Engine` namespace there is another namespace called `Input`. This namespace has everything input-related things. This is a namespace inside a namepsace, so accessing things from the `Input` namespace looks like this: `Engine::Input::<Namespace Member>`.

Similar to how you need to call `PrepareTerminal` before the game loop so you can print graphics, input also requires preparation so you can receive input events.

Inside the `Input` namespace there is a function called `Loop`, which is the input loop.

The input loop is a blocking loop which waits until every input press:

from `engine/input.cpp`:

```c++
void Engine::Input::Loop()
{
	while (true)
	{
		Get()
	}
}
```

This is the actual definition of `Engine::Input::Loop()`. What I am trying to say, is that by calling this function, you will not be able to continue your program, because it is an infinite loop that quits only when the program ends.

As you can see, the only operation this function does inside its while-loop is `Engine::Input::Get`, which returns until the player enters input, meaning `Get()` is also blocking.

So, if we want a game that works with a consistent TPS like our game does, then we can't simply call this before the base loop (beacuse it won't reach the base loop) nor can we call `Get()` inside it.

another thread; multithreading:

from `game.cpp`:

```c++
std::thread t_inputLoop(Engine::Input::Loop);
```

And now the input loop runs in a thread of its own.
