# Preamble

This document contains answers for potential questions.

[Why are `Collider`'s and `Texture`'s constructors normal functions?](#why-are-colliders-and-textures-constructors-normal-functions), [Why is there no predefined game loop?](#why-is-there-no-predefined-game-loop)

## Why are `Collider`'s and `Texture`'s constructors normal functions?

Using normal constructors was ambiguous to the coder and and the compiler in this case. The writing convention of `vector.resize(n)` and `vector[i].Constructor()`, where "vector" is a vector of `Texture`s or `Collider`s, is just the simple and easy alternative I found. I might change this method, maybe to a factory design.

## Why is there no predefined game loop?

It seems to me preferable forcing to write a game loop and understand how it works. Depends on how extensible a predefined game loop should be, it might require an amount of parameters and new features that is equivalent in code complexity to writing a game loop on the game side as is done now.