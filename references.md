# Introduction

`reference.md` includes direct information about each element of KTech.

Table of contents:

- [The simple structures](#the-simple-structures)

Rules of formatting:

- Monospace text (`example`) - something taken from code (such as a line of code and a name of a variable), or, a name of a file/directory.
- `Name` (first letter capitalized) - a structure (`struct`/`class`), a pointer to a function, or, a namespace.
- `Name()` (first letter capitalized and parentheses at the end) - a function.
- `name` (first letter lower case) - a variable.
- `NAME` (all letters capitalized) - macro (not used).

---

# The simple structures

Definition of what is a simple structure within the context of KTech:

> Primitive C++ `struct` that is used in a general fashion across the library. These structures don't include more functionality than simple constructors.

In front of you is the list of all the simple structures that come with, and are used by, KTech:

- [`RGB`](#rgb)
- `RGBA`
- `Point`
- `UPoint`
- `Cell`
- `CellA`

## `RGB`

### Synopsis

Represents a 24-bit RGB color.

### Members

- `uint8_t r` - 8-bit unsigned integer representing the red channel.
- `uint8_t g` - 8-bit unsigned integer representing the green channel.
- `uint8_t b` - 8-bit unsigned integer representing the blue channel.
- `inline constexpr RGB(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0)` (constructor)
  - Input: 3 8-bit unsigned integers (`uint8_t`), `red`, `green` and `blue`
  - Behaviour: Respectively initializes `r`, `g` and `b` with the given arguments.

`r`, `g`, and `b` represent together a single 24-bit RGB color value.

### Usages

- `Engine::Cell` - stores foreground and background colors using 2 `RGB` instances.
- `Engine::CellA::CellA()` - takes in an `RGB` and an `alpha` values to construct an `RGBA`.
- `Engine::Log()` - takes in an `RGB` representing the color in which the logged text will be printed as to the terminal.
- `Engine::RGBColors` - stores premade RGB colors as `constexpr RGB`s.

## `RGBA`

### Synopsis

Represents a 32-bit RGB with additional alpha channel color (RGBA).

### Members

- `uint8_t r` - 8-bit unsigned integer representing the red channel (primary color).
- `uint8_t g` - 8-bit unsigned integer representing the green channel (primary color).
- `uint8_t b` - 8-bit unsigned integer representing the blue channel (primary color).
- `uint8_t a` - 8-bit unsigned integer representing the alpha channel (opacity level).

`r`, `g`, `b` and `a` represent together a single 32-bit RGBA color value.

- `inline constexpr RGBA(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 255)`
  - Input:
  - Behaviour:
- `inline constexpr RGBA(RGB rgb, uint8_t alpha) : r(rgb.r), g(rgb.g), b(rgb.b), a(alpha) {}`
  - Input:
  - Behaviour:
