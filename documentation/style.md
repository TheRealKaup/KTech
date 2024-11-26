# Preamble

This document contains specifications for KTech's code. This document applies to the library itself (`ktech/*`), but not to code outside it (such as game examples in `examples/`).

The goal of this document is to force consistency to facilitate library usage. For instance, enforcing casing and certain variable prefixes assists in reaching this goal.

On the contrary, the scope of this document doesn't necessarily cover optimizations (such as avoiding 2D vectors). That's the responsibility of the coder and static analysis tool, and is not why this document was created to begin with.

A function implementation is allowed as long as it behaves "[as if](https://en.wikipedia.org/wiki/As-if_rule)" it follows the exact specifications.

## Outline

- [Preamble](#preamble)
    - [Outline](#outline)
- [Static Analysis](#static-analysis)
- [Naming](#naming)
- [Classes](#classes)
    - [Order of class members](#order-of-class-members)
    - [Class constructors](#class-constructors)
    - [Expected behavior of adder, remover, enterer and leaver functions](#expected-behavior-of-adder-remover-enterer-and-leaver-functions)
    - [Expected behavior of constructors and destructors of world structures](#expected-behavior-of-constructors-and-destructors-of-world-structures)
    - [Engine components](#engine-components)
    - [Other conventions about classes](#other-conventions-about-classes)
- [Source and header files](#source-and-header-files)
	- [Inclusion](#inclusion)
- [Other](#other)

# Static analysis

Code should be examined and corrected using Clang-Tidy, using the checks: `'-*, modernize-*, performance-*, readability-*, -readability-identifier-length, -readability-magic-numbers'`. The below specifications should be compatible with these checks. If they contradict, submit a GitHub issue, and Clang-Tidy's suggestions take precedence.

# Naming

## Underscores

**No underscores**, except when either:

- The underscore is added as a suffix to a name that is occupied by a keyword, and choosing a different name is inconvenient (e.g. `Keys::return_`).
- The underscore is part of an allowed prefix (e.g. `Texture::m_rPos`).

## Capitalization

- **Variables** and **enum values**: camelCase (e.g. `Object::m_parentLayer`).
- **Functions**, **classes**, **namespaces** and **enums**: PascalCase (e.g. `Input::RegisterRangedCallback()`).

## Member variables

Add "`m_`" prefix, except when either:

- The variable is a member of a class that doesn't have normal function members, (meaning, the class's members are mostly accessed from outside, thus the "m_" prefix is redundant). For example, member variables of `Point`, and of `Collision::CollisionData`.
- The variable is a public member of an engine component (meaning, it's more of a global variable than a member variable. Not adding the "m_" prefix makes this idea more intuitive and the code cleaner). For example, member variables of `Engine` itself and the variable `IO::input`.
- The variable is a reference or a pointer to `Engine`, or to a member variable of an engine component (similar idea behind the last case). For example, `Object::engine` and a hypothetical pointer to `Memory::maps`.

## Parameters

- Add "`p_`" prefix, except if in a header file (`.hpp`).
- Keep names verbose (e.g. good: `p_position`, bad: `p_pos`).

# Classes

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

If a member variable can only have one default variable (no matter which constructor constructed the object):

- Set the default value at the variable's declaration.
- And, do not initialize it in any of the constructors.

If any the following constructor parameters exist, they should go in the following order:

- `Engine` reference or pointer
- Parent structure to enter
- Anything else
- Optional name (last parameter)

## Expected behavior of adder, remover, enterer and leaver functions

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

Make member variables of engine components public only if they should be normally used by the user.

If an engine component becomes too big to be easily readable within 1 header and source files, split them into a new directory, like how `Input` has its own `engine/input/` directory.

## Other conventions about classes

If the class has only public members:

- Use `struct`.

Otherwise:

- Use `class`.

Virtualize the destructors of world structures and of any class that is expected to be derived from.

# Source and header files

## Inclusion

In a header file that defines a KTech class, this is the order of include directives:

- `#pragma once`
- `#define KTECH_DEFINITION`
- `#include "path_to/ktech.hpp"`
- `#undef KTECH_DEFINITION`
- `#include` internal dependencies, ordered by directory:
    - `utility/`
    - `basic/`
    - `world/`
    - `engine/`
    - `widgets/`
- `#include` external dependencies

For example:

```c++
#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../utility/id.hpp"
#include "../basic/cell.hpp"
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"

#include <string>
#include <vector>
```

In a source file that defines class members, this is the following format for inclusion:
- `#include` the class definition (header file)
- `#include` internal dependencies, ordered by directory:
    - `utility/`
    - `basic/`
    - `world/`
    - `engine/`
    - `widgets/`
- `#include` external dependencies

For example:

```c++
#include "output.hpp"

#include "../utility/internals.hpp"
#include "../utility/keys.hpp"
#include "../utility/rgbcolors.hpp"
#include "../basic/cell.hpp"
#include "../basic/upoint.hpp"
#include "../engine/engine.hpp"

#include <iostream>
```

# Other

Enumerations should typically be based on `uint8_t`, unless 1 byte isn't sufficient.

Scope curly brackets should have their own line:

```cpp
void Function()
{ // Good

}

void Function() { // Bad

}
```