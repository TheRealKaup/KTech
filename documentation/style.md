# Preamble

This document contains specifications for KTech's code.

Table of Contents:
- [Preamble](#preamble)
- [Naming](#naming)
- [Classes](#classes)
    - [Order of class members](#order-of-class-members)
    - [Class constructors](#class-constructors)
    - [Expected behavior of adder, remover, enterer and leaver functions](#expected-behavior-of-adder,-remover,-enterer-and-leaver-functions)
    - [Expected behavior of constructors and destructors of world structures](#expected-behavior-of-constructors-and-destructors-of-world-structures)
    - [Engine components](#engine-components)
    - [Other conventions about classes](#other-conventions-about-classes)
- [Source and header files](#source-and-header-files)
	- [Inclusion](#inclusion)

# Naming

**Variables** and **enum values** - in camelCase.

**Functions**, **classes**, **namespaces** and **enums** - the first letter of each word, including the first word, should be in uppercase.

**Member variables**:
- Add "m_" prefix (the following letter's capitalization shouldn't be affected), and,
- avoid additional underscores.
- Except when either:
    - The variable is a member of a class that doesn't have normal function members, (meaning, the class's members are mostly accessed from outside, thus the "m_" prefix is unnecessary). For example, member variables of `Point` and member variables of `Collision::CollisionData`.
    - The variable is a public member of an engine component (meaning, it's somewhat more of a global variable rather than a member variable. Not adding the "m_" prefix makes this idea more intuitive and the code cleaner). For example, member variables of `Engine` itself and the variable `IO::input`.
    - The variable is a reference or a pointer to `Engine`, or to a member variable of an engine component (similar idea behind the last case). For example, `Object::engine` and a hypothetical pointer to `Memory::maps`.

**Parameters**:
- If it's in the declaration of the function:
    - Avoid abbreviations.
- If it's in the separate definition of the function:
    - Add "p_" prefix (the following letter's capitalization shouldn't be affected), and,
    - make the rest of the name correspond with the member variable the parameter initializes (if it exists), and,
    - avoid additional underscores.

# Classes

## Order of class members

In the class definition, the public section should come first, then the protected section, then the private sections.

Within each section, the order of class members should be as listed:
- Enums
- Classes
- Variables:
    - Static variables
    - Engine data:
        - `Engine` reference or pointer
        - Personal `ID`
        - String name
        - Parent's `ID`
        - Children's `ID`s
        - Active status
    - External data (which are variables that are mostly processed by other classes, such as those that represent appearance and physical space)
    - Internal data (which are variables that are mostly processed by the class itself, usually anything that isn't external data)
- Constructors (including fake constructors like those in `Texture` and `Collider`)
- Destructor
- Operator overrides
- Normal functions:
    - Static functions
    - Virtual functions
    - Adders (e.g. `Layer::AddObject()`)
    - Removers (e.g. `Map::RemoveCamera()`)
    - Enterers (e.g. `Layer::EnterMap()`)
    - Leavers (e.g. `Object::LeaveLayer()`)
    - Checkers (e.g. `Input::Is()`)
    - Getters (e.g. `Texture::GetSize()`)
    - Setters (e.g. `Camera::Resize()`)
    - Anything else

The members' definitions should be in the same order they were declared in the class definition.

## Class constructors

The order of parameters in constructors of "simple classes" (to do: replace this vague term) should be the same order that the corresponding member variables are declared in.

If all constructors of a class initialize a member variable, or that memeber variable has a default constructor that is used in a constructor that makes said member variable irrelevant, don't set a defualt value for that member variable where it is declared.

For example:

```c++
struct KTech::Cell
{
    char c; // No default values here
    RGB f;
    RGB b;

    // First `c`, then `f`, then `b`, as this is a simple class.
    inline constexpr Cell(char character = ' ', RGB foreground = RGB(0, 0, 0), RGB background = RGB(0, 0, 0))
        : c(character), f{foreground}, b(background) {}
};
```

If a constructor accepts an `Engine` reference or pointer, that parameter should be the first one.

## Expected behavior of adder, remover, enterer and leaver functions

World structures (`Map`, `Camera`, `Layer`, `Object`, `UI` and `Widget`) add and remove from themselves child structures, and enter and leave parent structures. The functions that do these actions are called "adders", "removers", "enterers" and "leavers" respectively. This section specifies the expected behavior of these functions, as warranted by issue #84.

Since enterers and leavers (of child structure) simply call adders and removers (of parent structures) to do the actual adding and removing action. The end user can safely use both methods of adding and removing structures, and they may choose one solely on the basis of preference.

The "given structure" is provided as an ID.

### Add

- If the given structure does not exist in `Engine::Memory`:
    - Return false.
- If the given structure is already within this structure:
    - Return false.
- Call the leaver function of the given structure.
- Set the parent ID of the given structure to this structure's ID.
- Add the given structure to this child structures vector.
- Return true.

### Remove

- If the given structure is not within this structure:
    - Return false.
- If the given structure exists in `Engine::Memory`:
    - Set the parent ID of the given structure to `KTech::nullID`.
- Erase the given structure from this child structures vector.
- Return true.

### RemoveAll

- If there are no child structures:
    - Return false.
- For all child structures:
    - If the child structure exists in `Engine::Memory`:
        - Set the parent ID of the child structure to `KTech::nullID`.
- Clear child structures vector.
- Return false.

### Enter

- If the given structure is already the current parent:
    - Return false.
- If the given structure does not exist in `Engine::Memory`:
    - Return false:
- Call this structure's leaver function, and return its returned value.

### Leave

- If the current parent exits in `Engine::Memory`:
    - Call the remove function of current parent, and return its returned value.
- Set the parent ID to `KTech::nullID`.
- Return true.

## Expected behavior of constructors and destructors of world structures

### Constructor

When constructed, world structures are responsible for adding themselves to `Engine::Memory`, and for entering other world structors.

If the constructor accepts a parent map to enter:
- Add this structure to `Engine::Memory` (i.e. `engine.memory.container.Add(this);`).

If the constructor doesn't accept a parent map to enter, call the constructor that doesn't, and enter the given parent.

### Destructor

When destructed, world structures are responsible for removing themselves to `Engine::Memory`, for leaving their parents, and for removing child structures.

- Call this `RemoveAll` function.
- Call this leaver function.
- Remove this from `Engine::Memory` (i.e. `engine.memory.container.Remove(m_id);`).

## Engine components

Make member variables of engine components public only if they should be normally used by the user.

If an engine component becomes too big to be easily readable within 2 singular header and source files, split them within a new directory, similar to how `Input` has its own `engine/input/` directory.

## Other conventions about classes

Use the `struct` keyword for a class that has no protected or private members. Otherwise, use the `class` keyword.

Virtualize the destructors of world structures and of any class that is expected to be derived from, in order to avoid memory leaks.

# Source and header files

## Inclusion

In a header file that defines a KTech class, this is the following format for inclusion:
- `#pragma once`
- New line
- `#define KTECH_DEFINITION`
- `#include "path_to/ktech.hpp"` (normally `"../ktech.hpp"`)
- `#undef KTECH_DEFINITION`
- `#include` KTech dependencies by the following directory order, then by alphabetical order:
    - From `utility/` directory
    - From `basic/` directory
    - From `world/` directory
    - From `engine/` directory
    - From `widgets/` directory
- New line
- `#include` external dependencies in alphabetical order

For example:

```c++
#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../utility/cr.hpp"

#include <vector>
```

In a source file that defines class members, this is the following format for inclusion:
- `#include` the class members' declarations (meaning, the class definition)
- New line
- `#include` KTech dependencies by the following directory order, then by alphabetical order:
    - From `utility/` directory
    - From `basic/` directory
    - From `world/` directory
    - From `engine/` directory
    - From `widgets/` directory
- New line
- `#include` external dependencies in alphabetical order

For example:

```c++
#include "io.hpp"

#include "../utility/keys.hpp"
#include "../utility/rgbcolors.hpp"
#include "../basic/cell.hpp"
#include "../basic/cella.hpp"
#include "../basic/upoint.hpp"
#include "../engine/engine.hpp"

#include <iostream>
```
