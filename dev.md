I deicided to create this file for more trackable and readable development logs, notes and to-dos.

This does not replace the git commit messages.

# Overlapping

## 24/6/2023 17:37

I believe the engine used to track overlapping through the `Object::Move()` function, though that functionality is long gone because it was too difficult and complicated to do.

I now want to return it because it is useful and much more intuitive, easier and resource efficient than checking for overlapping each tick, which is what the engine currently is designed to do (though that functionality is also gone since the simple colliders update).

I already know collision is very much not finished and requires some touches, but I forgot just how much is missing. Now writing the tutorials, which is a way for me to see that everything is up to standard, overlapping is definitely the most missing part of collision, out of complex colliders, better user writing conventions, and I think that's it.

# Simple textures & collider constructors

## 24/6/2023 18:12

Make textures and colliders behave the same class-wise:
- Simple texture option like simple collider.
- Collider constructors like texture constructors

This also makes both more intuitive.

# Better print

## 24/6/2023 18:16

The idea is a more efficient print functions that knows how to adapt as fast as possible to new terminal sizes.

Maybe I should skip this idea and once again use a library that already works.