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

Entities (`Map`, `Camera`, `Layer`, `Object`, `UI` and `Widget`) add and remove from themselves sub (child) entities, and enter and leave super (parent) entities. The functions that do these actions are called "adders", "removers", "enterers" and "leavers", respectively. This section used to specify the exact behavior of these functions, as warranted by issue [issue #84](https://github.com/TheRealKaup/KTech/issues/84). But because [issue #127](https://github.com/TheRealKaup/KTech/issues/127) covered a lot of the behavior, this section has been minimized to cover what the adder and remover wrappers to `KTech::Sub` and the usual enterers and leavers should do, and the exact behavior was moved to the Doxygen documentation of `KTech::Sub`.

Enterers and leavers (of sub entities) call the adders and removers (of parent entities) to do the actual adding and removing. The library user may choose either calling the functions of the sub entity or parent entity based on their preference, and expect similar behavior.

Entities are passed around as `KTech::ID`s. Unless required explicitly, do not add anymore argument validations (like ensuring the parent ID is in memory).

### Parent entity

- The adder, remover, and remove-all functions, should call and return the value of the appropriate member `Sub`'s `Add()`, `Remove()` and `RemoveAll()`, respectively.
- The destructor should call its entity's remove-all function. If there are multiple member `Sub`s, then all remove-all functions must be called.

### Sub entity

- The enterer should get the given parent ID from memory, and call its adder.
- The leaver should check that its member parent ID is not `nullID`. If it isn't, then it should get the parent from memory and call its remover.
- There should be a constructor that accepts a parent entity and one that doesn't. The constructor that does should be based on the one that doesn't, and it should call the entity's enterer.
- The destructor should call the entity's leaver.

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

/*!
	@file
	@brief Define KTech::ABC class
	@author Author name
	@author Another author name...
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

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

/*!
	@file
	@brief Define KTech::ABC members
	@author Author name
	@author Another author name...
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "output.hpp" /* The class of the members being defined */

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

# Documentation

Document everything using Doxygen.

If possible, always place the Doxygen comment next to (in front of) the object being documented. This applies to functions, data members, classes, and more. This is contrary to placing the comment somewhere else and using a structural command to specify what it is you are documenting, which is only allowed when documenting things that must be documented in this way.

The comment style is:

```cpp
/*!
	@brief Like this.
*/
```

- `/*!` instead of `/**` because it's clearer this way that this is a Doxygen comment.
- `@` instead of `\` because it looks better.
- No preceding `*` in each line because it's easier to write the comment without.

And generally speaking, keep the structure of new comments similar in that of existing comments.

# Other

- Enumerations should typically be based on `uint8_t`, unless 1 byte isn't sufficient.
- Remember to use `[[deprecated("reason")]]`, `[[nodiscard]]`, and other attributes accordingly.
