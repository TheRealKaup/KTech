I deicided to create this file for more trackable and readable development logs, notes and to-dos.

This does not replace the git commit messages.

# To do

|Title|Description|Status|Original log|
|-|-|-|-|
|Overlapping|Overlap functionality with event calling|Done but untested|[Overlapping](#overlapping)|
|Consistent `Texture` and `Collider` classes|Add simple mode to `Texture` and creation functions to `Collider`|In progress, missing complex `Collider` and simple `Texture` functionality|[Simple textures & collider constructors](#simple-textures--collider-constructors)|
|Optimizing memory allocation use in `ExpandMovementTree()`|Reduce the use of `std::vector::push_back()`|Done|[Memory](#memory)|
|Tutorials|Finish writing [the tutorails](tutorials.md)|Currently writing [collision](tutorials.md/#collision), haven't yet started writing [widgets](tutorials.md/#widgets), [audio](tutorials.md/#audio) and [putting it all together](tutorials.md/#putting-it-all-together), still requires editing||

# Overlapping

## 24/6/2023 17:37

I believe the engine used to track overlapping through the `Object::Move()` function, though that functionality is long gone because it was too difficult and complicated to do.

I now want to return it because it is useful and much more intuitive, easier and resource efficient than checking for overlapping each tick, which is what the engine currently is designed to do (though that functionality is also gone since the simple colliders update).

I already know collision is very much not finished and requires some touches, but I forgot just how much is missing. Now writing the tutorials, which is a way for me to see that everything is up to standard, overlapping is definitely the most missing part of collision, out of complex colliders, better user writing conventions, and I think that's it.

## 24/6/2023 20:02

What's the goal:
- `OnOverlap` callback
- `OnOverlapExit` callback
- `CheckOverlap()`

The reason CheckOverlap() is still needed is for the times the user directly sets `Object::pos` or `Collider:pos`.

The overlap states are stored in 3 vectors, a vector of pointers to objects which are currentyly overlapping, a vector with the same length of indexes for the specific colliders overlapping with the object, and another vector with the same length of indexes of the colliders of the object overlapping with the other objects.

`CheckOverlap()` simply updates the overlapping state.

`OnOverlapEnter` is called when there is a new overlap state.

`OnOverlapExit` is called when an overlap state is removed.

I wonder if I should add to `Object::Move()` functionality to update overlap states while checking collision, or after checking collisions, or should it not do anything and let the user decide when to call `CheckOverlap()`.

- Option 1 pros: could be the most resource efficient.
- Option 1 cons: massive thing to implement
- Option 2 pros: simple, easiest thing to do for the user
- Option 2 cons: the user can't decide whether to skip overlapping checking
- Option 3 pros: simple, the user can decide whether to skip overlapping checking
- Option 3 cons: could be annoying to call for the user

Perhaps a 4th option is to add a boolean parameter to `Move()` which determines wether to call `CheckOverlap()` inside `Move()` or not.

I doubt the performance difference between calling `CheckOverlap` in `Move()` after the collision check part and implementing overlap check inside the collision check part is much different.

Also, this overlapping system won't offer information about which object overlapped into which, so maybe I need to think about this more.

## 24/6/2023 21:34

After thinking about it, this system is not up to standard. There shouldn't be a `CheckOverlap()` function, `Engine::Move()` should do it all and also specifiy which object overlapped which.

## 24/6/2023 22:36

Looking at `Move()` I see that at the successefuly moving part of the code, positions are updated while events are called, and not all of the positions are first updated and then events are called. I think the second option makes more sense, so fix that while you are at it. 

## 25/6/2023 13:59

Just added 4 more vectors to `Object::ExpandMovementTree()` and I'm about to add 4 more. Overlapping is serious business. In my opinion this just doubled the complexity of `Move()`. Basically, 4 of these 8 new vectors are for new overlap states, and the other 4 are for exiting overlap states. In `ExpandMovementTree()`, if the potential collision result is overlap, then I should compare colliders in 2 ways; before movement and after movement, so I can tell if an overlap exited. I now realize there is also no index to tell which is the other object from `Object::otherOverlappingObjects` is the relevant one in the context of overlap event calls. I should add this.

## 25/6/2023 16:55

Finished writing but I am yet to test. I realized that there is no reason to store the overlap states in the object, and instead, `Move()` checks before movement and after movement overlap states which fulfills both overlap exit and overlap enter data. Basically, now `Object` and collision is nice and tidy.

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

## 25/6/2023 18:07

The engine desperately needs a better terminal adaptation update.

# Memory

## 27/6/2023

2 issues:
- In `ExpandMovementTree()` I'm constantly pushing into the vectors, which is incredibly costy.
- Pointer managment in the engine sucks. There is no clear and cheap way to transfer an object between layers or maps. Not sure if unique pointers are much of a solution.

## 28/6/2023 12:41

Solutions for `ExpandMovementTree()` vector issue:
- Divide the amount of vectors by 4 by replacing same-sized vectors with sctructured vectors. For example: `vector pushingObjects, objectsToPush, pushingColliders, collidersToPush` -> `vector pushData = { {object, object, size_t, size_t}, ... }`.
- Private variables inside `Object` to completely avoid any vectors or maps, resets those variables at the start and end of `Move()`.

## 28/6/2023 14:05

Went with the first solution. Also, `ExpandMovementTree()` is not a part of the `Engine` namespace anymore. There is no reason for it to be there, the only thing it could do is confuse the user. So, what I have done now cut the allocation occurrences by 4 times.