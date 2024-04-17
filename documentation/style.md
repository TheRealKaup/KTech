# Preamble

This document contians specifications for KTech's coding style.

Table of Contents:
- [Preamble](#preamble)
- [Naming](#naming)

# Naming

**Variables** and **enum values** - in camelCase.

**Functions**, **classes** and **namespaces** - the first letter of each word, including the first word, should be in uppercase.

**Variable members**:
- Add "m_" prefix (the following letter's capitalization shouldn't be affected), and,
- Avoid additional underscores.
- Except when either:
    - The variable is a member of a structure that has no normal function members, (meaning, the structure's members are inherently accessed from outside, thus the "m_" prefix is unnecessary). For example, variable members of `Point` and variable members of `Collision::CollisionData`.
    - The variable is public and created once per `Engine` instance (meaning, it's somewhat more of a global variable rather than a member variable. Not adding the "m_" prefix makes this idea more intuitive and the code cleaner). For example, variable members of `Engine` itself and the variable `IO::input`.
    - The variable is a reference or a pointer to `Engine`, or to a variable that is created once per `Engine` instance (similar idea behind the last case). For example, `Object::engine` and a hypothetical pointer to `Memory::maps`.
