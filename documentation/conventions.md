# Outline

This document describes KTech's code conventions and style guide. This document applies to the library itself (`ktech/*`), but not necessarily to code outside it (such as game examples in `examples/`).

The "[as-if rule](https://en.wikipedia.org/wiki/As-if_rule)" applies here where it makes sense.

- [Outline](#outline)
- [Linter and formatter](#linter-and-formatter)
- [Naming](#naming)
- [Classes](#classes)
  - [Basic structures](#basic-structures)
  - [Order of class members](#order-of-class-members)
  - [Class constructors](#class-constructors)
  - [Expected behavior of adder, remover, enterer and leaver functions](#expected-behavior-of-adder-remover-enterer-and-leaver-functions)
    - [Add](#add)
    - [Remove](#remove)
    - [RemoveAll](#removeall)
    - [Enter](#enter)
    - [Leave](#leave)
  - [Expected behavior of constructors and destructors of world structures](#expected-behavior-of-constructors-and-destructors-of-world-structures)
    - [Constructor](#constructor)
    - [Destructor](#destructor)
  - [Engine components](#engine-components)
  - [Other conventions about classes](#other-conventions-about-classes)
- [Source and header files](#source-and-header-files)
- [Other](#other)

# Linter and formatter

Before pushing code, it should be:

- Linted with Clang-Tidy and the provided `.clangd` configuration. All warnings should be corrected.
- Formatted with ClangFormat (I have no idea why LLVM didn't put a dash between the Clang and the Format but whatever) and the provided `.clang-format` configuration.

These tools complete KTech's rules regarding conventions and style. They detect and enforce many more rules that are detailed in their configuration files (and not here). I highly recommend looking at those files for a fuller picture. If you encounter a lint/format that contradicts with something written here, or that just seems wrong, feel free to submit a GitHub issue!

# Naming

- **No underscores**, except when either:
    - The underscore is added as a suffix to a name that is occupied by a keyword, and choosing a different name is inconvenient (e.g. `Keys::return_`).
    - The underscore is part of an allowed prefix (e.g. `Texture::m_rPos`).
- Keep parameter names verbose (e.g. good: `p_position`, bad: `p_pos`).

# Classes

## Basic structures

Data members of basic structures (classes with very little functionality in them, that are just supposed to carry data, and could have been simple aggregate types, for example, `RGB`, or anything else defined in `ktech/basic/`), are exempt (along with some other data members of other classes, see `.clangd` settings relating to Clang-Tidy) from the `m_` prefix. To make Clang-Tidy recognize that a file is defining a basic structure (and not warn about the missing prefix), the file path should be added to the list of basic structures in `.clangd`.

## Order of class members

In a class definition, the public section should come first, then the protected section, and lastly the private section.

Within each section, the order of class members should be:

- Enums & Classes
- Variables:
    - Static variables
    - `Engine` reference or pointer
    - Anything else (in an order that minimizes padding)
- Constructors (including fake constructors like those in `Texture` and `Collider`)
- Destructor
- Operator overrides
- Normal functions:
    - Static functions
    - Virtual functions
    - Anything else
- `friend` declarations

Members should be defined (i.e. in the class source file) in the order they were declared (i.e. in the class header file).

## Class constructors

If a member variable is initialized to the same value no matter which constructor constructed its parent object, then it should be initialized at its declaration (contrary to initializing it at the constructor, where if there are multiple constructors, the initialization value gets duplicated).

If any the following constructor parameters exist, they should go in the following order:

- `Engine` reference or pointer
- Parent structure to enter
- Anything else
- Optional name (last parameter)

## Expected behavior of adder, remover, enterer and leaver functions

Note that this section will hopefully be removed or shortened with the closing of [issue #127](https://github.com/TheRealKaup/KTech/issues/127), by means of generic programming/class inheritance.

World structures (`Map`, `Camera`, `Layer`, `Object`, `UI` and `Widget`) add and remove from themselves child structures, and enter and leave parent structures. The functions that do these actions are called "adders", "removers", "enterers" and "leavers", respectively. This section specifies the expected behavior of these functions, as warranted by issue #84.

Enterers and leavers (of child structures) call the adders and removers (of parent structures) to do the actual adding and removing. The library user may choose either directions based on preference, and expect similar behavior.

The "given structure" is provided as a `KTech::ID<T>`.

### Add

- If the given structure does not exist in `Engine::Memory`:
    - Return false.
- If the given structure is this structure itself:
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
- Return true.

### Enter

- If the given structure does not exist in `Engine::Memory`:
    - Return false:
- If the given structure is this structure itself:
     - Return false.
- If the given structure is already the current parent:
    - Return false.
- Call this structure's leaver function, and return its returned value.

### Leave

- If the current parent exits in `Engine::Memory`:
    - Call the remove function of current parent, and return its returned value.
- Set the parent ID to `KTech::nullID`.
- Return true.

## Expected behavior of constructors and destructors of world structures

Note that this section will hopefully be removed or shortened with the closing of [issue #127](https://github.com/TheRealKaup/KTech/issues/127), by means of generic programming/class inheritance.

### Constructor

When constructed, world structures are responsible for registering themselves at `Engine::Memory`, and sometimes also for entering other world structures.

If the constructor does not accept a parent structure to enter:

- Add this structure to `Engine::Memory` (i.e. `engine.memory.container.Add(this);`).

Otherwise:

- Call the constructor that doesn't accept a parent to enter, if such overload exists.
- Enter the given parent.

### Destructor

When destructed, world structures are responsible for removing themselves from `Engine::Memory`, and sometimes for leaving their parents and removing their child structures.

- Call this `RemoveAll` function, if this can have child structures.
- Call this `Leave` function, if this can have a parent structure.
- Remove this from `Engine::Memory`.

## Engine components

If an engine component becomes too big to be easily readable within 1 header and source files, split them into a new directory, like how `Input` has its own `engine/input/` directory.

## Other conventions about classes

- Make member variables of engine components public only if they should be normally used by the user.
- Use `struct` if the class has only public members. Use  `class` otherwise.
- Virtualize the destructors of world structures and of any class that is expected to be derived from.

# Source and header files

Header files should start like this (without the comments):

```c++
/* Copyright notice should go here */

#pragma once

#define KTECH_DEFINITION /* KTech namespace */
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../basic/cell.hpp" /* Internal includes */
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"
#include "../utility/id.hpp"

#include <limits> /* External includes */
#include <string>
#include <vector>

/* Main content (class/namespace/whatever definition) */
```

Similarly for source files:

```c++
/* Copyright notice should go here */

#include "output.hpp" /* The class of the member functions being defined */

#include "../utility/internals.hpp" /* Internal includes */
#include "../utility/keys.hpp"
#include "../utility/rgbcolors.hpp"
#include "../basic/cell.hpp"
#include "../basic/upoint.hpp"
#include "../engine/engine.hpp"

#include <iostream> /* External includes */

/* Main content (member function definitions) */
```

Note that Clang-Format takes responsibility of ordering the includes lexicographically.

# Other

- Enumerations should typically be based on `uint8_t`, unless 1 byte isn't sufficient.
- Remember to use `[[deprecated("reason")]]`, `[[nodiscard]]`, and other attributes accordingly.
