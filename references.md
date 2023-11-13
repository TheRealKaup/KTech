# References

`references.md` (this markdown file) includes direct information about each element of KTech.

Everything here is included in the header file of the library (`engine.hpp`), so, in order to include everything here in your project, simple add:

```c++
#include <path_to_engine_directory/engine.hpp>
```

Table of contents:

- [The Simple Structures](#the-simple-structures)

Rules of formatting:

- Monospace text (`example`) - something taken from code (such as a line of code and a name of a variable), or, a name of a file/directory.
- `Name` (first letter capitalized) - a structure (`struct`/`class`), a pointer to a function, or, a namespace.
- `Name()` (first letter capitalized and parentheses at the end) - a function.
- `name` (first letter lower case) - a variable.
- `NAME` (all letters capitalized) - macro (not used).

---

# The Simple Structures

The simple structures are all of the primitive C++ `struct`s that are used in a general fashion across the library. These structures don't include more functionality than their constructors.

In front of you is the list of all the simple structures that come with, and are used by KTech:

- [`RGB`](#rgb)
- [`RGBA`](#rgba)
- `Point`
- `UPoint`
- `Cell`
- `CellA`

## `RGB`

### Description

Represents a 24-bit RGB color.

### Members

- `uint8_t r` - 8-bit unsigned integer representing the red channel.
- `uint8_t g` - 8-bit unsigned integer representing the green channel.
- `uint8_t b` - 8-bit unsigned integer representing the blue channel.
- `RGB()` (default constructor)
  - Syntax:
    ```c++
    constexpr RGB(
      [In] uint8_t red = 0,
      [In] uint8_t green = 0,
      [In] uint8_t blue = 0
    )
    ```
  - Parameters:
    - `red` - red channel, initializes `this->r`.
    - `green` - green channel,  initializes `this->g`.
    - `blue` - blue channel, initializes `this->b`.

## `RGBA`

### Description

Represents a 32-bit RGB with additional alpha channel color (RGBA).

### Members

- `uint8_t r` - 8-bit unsigned integer representing the red channel (primary color).
- `uint8_t g` - 8-bit unsigned integer representing the green channel (primary color).
- `uint8_t b` - 8-bit unsigned integer representing the blue channel (primary color).
- `uint8_t a` - 8-bit unsigned integer representing the alpha channel (opacity level).

`r`, `g`, `b` and `a` represent together a single 32-bit RGBA color value.

- `inline constexpr RGBA(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 255)`
  - `uint8_t red = 0` - initializes
- `inline constexpr RGBA(RGB rgb, uint8_t alpha)`
  - `RGB rgb` - initializes `r`, `g`, and `b`.
  - `uint8_t alpha` - initializes `a`. 
