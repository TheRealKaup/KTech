# Table of Contents:

- [Table of Contents](#table-of-contents)
- [Introduction to Quick Start Guide](#introduction-to-quick-start-guide)
- [Key Concepts](#key-concepts)
  - [The File System](#the-file-system)
  - [World Hierarchy](#world-hierarchy)
  - [Engine](#engine)
  - [Game Loop](#game-loop)
  - [Widgets](#widgets)
- [Starting a Project](#starting-a-project)
- [Building](#building)
  
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

### `Map`

Maps are completely disconnected from each other. They are a good way to divide your game to levels, for example. Maps contain a list of their layers.

### `Layer`

Layers are desigend to be loaded together. Layers contain a list of their objects.

When you render a map, it will render all of the layers in it, in the order they where added to the map. Meaning, textures in the first layer will be rendered first, and textures in the second layer will be rendered on top.

Physics-wise, objects interact only with other objects in the same layer.

### `Object`

Objects comprise textures and collider that behave relative to their parent object's position. Textures represent the object's appearance, and colliders represent the object's physical space.

### `Texture`

There are simple and complex textures, that both comprise of `CellA` values, beacuse textures are meant to be rendered for the terminal. Complex colliders are 2D arrays. Simple textures are instead rectangles, and thus are faster to render and have smaller memory usage, but they are of course less capable than complex colliders.

### `Colliders`

There are simple and complex colliders, in a similar manner to textures. Complex colliders are made of 2D array of `bool`s to represent where there is collider and where there isn't. Both complex and simple colliders have a `type` value, which is used to determine their collision result with other colliders.

Collision between objects can result in a push, a block, or an overlap event.

### `Camera`

Cameras are able to render a `Cell` based image that can be printed to the terminal, given layers that contain objects with textures.

## `Engine`

The `Engine` class comprises components that make the game run. These components manage different aspects, and are:
- `Collision` - Manages movement of objects and calculates collision events.
- `Audio` - Manages audio sources and playing audio
- `Time` - Manages invocations and provides game loop utilities
- `IO` - Manages user input and printing graphics
- `Memory` - Manages linking the world structure together

### `Collision`

Gets requests to move a certain object, calculates how its movement would look like, and based on that updates positions of objects and calls the relevant collision events on the relevant objects. Object movement and `Collision` work on demand and immediately return results and update the positions of objects after being called.

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

`Memory` is the serialization solution for the world hierarchy. It contains a `Container` for each of the storable structures - `Map`, `Layer`, `Camera` and `Object`.

The containers receive pointers to structures and return an `ID` that can be used to reach the structure again.

## Game Loop

The game loop in this context is a loop that attempts to run at a consistent iteration speed, which are called game ticks. Each tick usually does the same tasks, such as processing user input (which causes other events such as object movements), rendering, drawing and printing.

The `Engine` class nor any other part of the library provides a premade game loop function. Instead, the user constructs one with the functions provided in the `Time` component of `Engine`.

## `Widget`s

Widgets are UI elements derived from the `Widget` class which is derived from the `Object` class.

In the KTech ecosystem they are unrequired additions that you can include in your game as you wish.

They collect user input automatically and can be enabled and disabled with their universal `Select()` and `Deselect()` functions.

The currently available widgets are:
- Button
- Integer input field
- String input field
- Switch

# Starting a Project
