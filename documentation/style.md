# Preamble

This document contains specifications for KTech's coding style.

Table of Contents:
- [Preamble](#preamble)
- [Naming](#naming)
- [Classes](#classes)
    - [Order of class members](#order-of-class-members)
    - [Class constructors](#class-constructors)
    - [Other conventions about classes](#other-conventions-about-classes)
- [Source and header files](#source-and-header-files)
	- [Inclusion](#inclusion)

# Naming

**Variables** and **enum values** - in camelCase.

**Functions**, **classes**, **namespaces** and **enums** - the first letter of each word, including the first word, should be in uppercase.

**Member variables**:
- Add "m_" prefix (the following letter's capitalization shouldn't be affected), and,
- Avoid additional underscores.
- Except when either:
    - The variable is a member of a class that has non-static normal function members, (meaning, the class's members are mostly accessed from outside, thus the "m_" prefix is unnecessary). For example, member variables of `Point` and member variables of `Collision::CollisionData`.
    - The variable is public and created once per `Engine` instance (meaning, it's somewhat more of a global variable rather than a member variable. Not adding the "m_" prefix makes this idea more intuitive and the code cleaner). For example, member variables of `Engine` itself and the variable `IO::input`.
    - The variable is a reference or a pointer to `Engine`, or to a variable that is created once per `Engine` instance (similar idea behind the last case). For example, `Object::engine` and a hypothetical pointer to `Memory::maps`.

**Parameters**:
- If it's in the declaration of the function:
    - Make the rest of the name generally clear and avoid abbreviations.
- If it's in the separate definition of the function:
    - Add "p_" prefix (the following letter's capitalization shouldn't be affected), and,
    - Make the rest of the name correspond with the member variable the parameter initializes (if it exists), and,
    - Avoid additional underscores.

# Classes

## Order of class members

In the class definition, the public section should come first, then the protected section, then the private sections.

Within each section, the order of class members should be as listed:
- Enums
- Classes
- Variables:
    - Engine data:
        - `Engine` reference or pointer
        - Personal `ID`
        - String name
        - Parent's `ID`
        - Children's `ID`s
        - Active status
    - External data (which are variables that are mostly used and managed by other classes, such as those that represent appearance and physical space)
    - Internal data (anything that isn't external data as described above)
- Constructors (including fake constructors like those in `Texture` and `Collider`)
- Destructor
- Operator overrides
- Normal functions:
    - Static functions
    - Virtual functions
    - Adders (e.g. `Layer::AddObject()`)
    - Removers (e.g. `Map::RemoveCamera()`)
    - Enterers (e.g. `Layer::EnterMap()`)
    - Getters (e.g. `Texture::GetSize()`)
    - Setters (e.g. `Camera::Resize()`)
    - Anything else

The members' definitions should be in the same order they were declared in the class definition.

## Class constructors

The order of parameters in constructors of "simple classes" should be the same order that the corresponding member variables are declared in.

If at least one constructor provides a default parameter for a member variable, that member variable shouldn't have a default value where it is declared.

For example:

```c++
struct KTech::Cell
{
    char c; // No default values here
    RGB f;
    RGB b;

    // First `c`, then `f`, then `b`.
    inline constexpr Cell(char character = ' ', RGB foreground = RGB(0, 0, 0), RGB background = RGB(0, 0, 0))
        : c(character), f{foreground}, b(background) {}
};
```

## Other conventions about classes

Use the `struct` declaration for a class that has no protected or private members. Otherwise, use the `class` declaration.

# Source and header files

## Inclusion

In a header file that defines a class, this is the following format for inclusion:
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
