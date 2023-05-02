/*
Features:

- Collision checking
	When objects move will check for surroundings and react with user defined collider types (what can push / go through what)
	(DONE)

- Gravity
	Applies force[s] in a straight or point centered direction or to every object which has gravity enabled, given by mass and velocity

-Called functions
	Calls user defined functions when an event from a generous selection happens, also suppliend all types of data.
- Path finding when an object moves but can't go through, or gets stuck.

- Bounciness
	When an object collides, bounciness will be applied if enabled

The physics engine in KCGE has almost all the features a normal popular 2D game engine has (for example Unity), but it's only precise on int level (because of graphics), and doesn't have rotation.
I will try to get the most out of the limited graphics.
*/

#include "engine.hpp"

using namespace Engine;

// Does not check if relevancy (such as if both are in the same layer or have the collider active), just compares colliders on position. This is for a specific part in the Move function.
bool Engine::AreObjectsOverlapping(Object* objA, Object* objB, bool ignoreIrrelevancy)
{
	return false;
}

void Engine::Object::ExpandMovementTree(Vector2D dir, std::vector<Object*>* pushingObjects, std::vector<Object*>* objectsToPush, std::vector<unsigned>* pushingColliders, std::vector<unsigned>* collidersToPush, std::vector<Object*>* blockedObjects, std::vector<Object*>* blockingObjects, std::vector<unsigned>* blockedColliders, std::vector<unsigned>* blockingColliders)
{
	// Only supporting SIMPLE COLLLIDERS.

	Object* oObj; // other object
	Collider* collider;
	Collider* otherCollider;

	// Other objects
	for (size_t o = 0; o < parentLayer->objects.size(); o++)
	{
		oObj = parentLayer->objects[o];

		for (int i = 0; i < pushingObjects->size(); i++) {
			// pushingObjects, objectsToPush (and blockedObjects (those are in pushingObjects or objectsToPush anyways)) already desire to move and count as a part of the tree move.
			if (oObj == (*pushingObjects)[i] || oObj == (*objectsToPush)[i]) {
				oObj = this; // Stupid way to exit the other for-loop but whatever.
			}
		}
		if (oObj == this)
			continue;

		// In the recursion for-loop:
		// If got to this point, oObj is:
		// a) A new object (yet to be collided).
		// b) A blocking object (that the moving object needs a different object to push).

		unsigned char result = 2; // Default - overlap, IGNORE.
		unsigned c = 0; // collider index
		unsigned oc = 0; // other object's collider index
		unsigned originallyBlockedColliderI = 0;
		unsigned originallyBlockingColliderI = 0;


		for (c = 0; c < colliders.size() && result != 1/*continue only if yet to push*/; c++) // For moving object's colliders
		{
			collider = &colliders[c];

			if (!collider->active || collider->size.x == 0 || collider->size.y == 0)
				continue; // Collider is 0

			for (oc = 0; oc < oObj->colliders.size() && result != 1/*continue only if yet to push*/; oc++) // Other object's colliders
			{
				otherCollider = &oObj->colliders[oc];

				if (!otherCollider->active || otherCollider->size.x == 0 || otherCollider->size.y == 0)
					continue; // Other object's collider is 0

				// Get the potential result of the collision using the collider and the other object's collider types.
				unsigned char collisionResult = 2; // In this context, there is no difference between overlap and no collision at all. So 2 can be the default.
				if (collider->type >= 0 && collider->type < colliderTypes.size())
					if (otherCollider->type >= 0 && otherCollider->type < colliderTypes[collider->type].size())
						collisionResult = colliderTypes[collider->type][otherCollider->type];
				// If the collision result is 2 (overlap or types are invalid) then the collision of the objects doesn't matter, so continue.
				// Also, if prior to comparing these current colliders in the loop a BLOCK was already found, then there is no reason to check collision if these 2 colliders...
				// ... could potentially block each other too.
				if (collisionResult == 2 || (result == 0 && collisionResult == 0))
					continue;

				// Reached this point if:
				// 1. Other object's collider could potentially block the object (if not already understood that is blocking)
				// 2. Other object's collider could potentially be pushed.

				// Proceeding to check collision.

				int cldrPosX = collider->pos.x + pos.x + dir.x;
				int oCldrPosX = otherCollider->pos.x + oObj->pos.x;
				int cldrPosY = collider->pos.y + pos.y + dir.y;
				int oCldrPosY = otherCollider->pos.y + oObj->pos.y;
				if (cldrPosX < oCldrPosX + (int)otherCollider->size.x && cldrPosX + (int)collider->size.x > oCldrPosX
					&& cldrPosY < oCldrPosY + (int)otherCollider->size.y && cldrPosY + (int)collider->size.y > oCldrPosY)
				{
					// Collision!
					result = collisionResult;

					// There can only be one time (for each other object) where a block situation can get to this point in the code.
					// That means that c and oC represent the original blocked and blocking colliders, which can be used for user-defined functions calling.
					if (result == 0) {
						originallyBlockedColliderI = c;
						originallyBlockingColliderI = oc;
					}
				}
				// If result is push (==1), then the loops of: for(colliders){ for(other object's colliders) }, will exit and return to the other object's loop.
			}
		}

		// What happened until this point in the current loop's iteration:
		// 1. Started with one of the other objects in the layer.
		// 2. Compared collision with every combination of the other object's colliders and the moving object's colliders.
		// 3. a) If there was a push, then it exited immediately and now we are at this point with a PUSH reuslt.
		//    b) Or, a BLOCk was found and no PUSH was found, so the other object is BLOCKING.
		//    c) Otherwise, the objects didn't collider at all in any sort of way, such as an overlap or didn't touch from the first place.

		// If result == 0 (block), then add the other object, collider and other collider's indexes to a vector for later.
		if (result == 0)
		{
			blockedObjects->push_back(this);
			blockingObjects->push_back(oObj);
			blockedColliders->push_back(originallyBlockedColliderI); // self explanatory
			blockingColliders->push_back(originallyBlockingColliderI);
		}
		// If result == 1 (push), then blah blah blah.
		else if (result == 1)
		{
			pushingObjects->push_back(this);
			objectsToPush->push_back(oObj);
			collidersToPush->push_back(c); // c and oc were the last indexes in the for loops above, which we just exited since it found PUSH, so using them works.
			pushingColliders->push_back(oc);
		}
	}
}

bool Object::Move(Vector2D dir)
{
	// Vectors related to the pushing and pushed objects:
	std::vector<Object*> pushingObjects; // The objects that push the objects that will be pushed.
	std::vector<Object*> objectsToPush; // Objects that will be pushed.
	std::vector<unsigned> pushingColliders; // Colliders that push the colliders that will be pushed (used for event calling).
	std::vector<unsigned> collidersToPush; // Colliders that will be pushed (used for event calling).
	// Vectors related to the blocked and blocking objects:
	std::vector<Object*> blockedObjects; // The objects that are blocked.
	std::vector<Object*> blockingObjects; // The objects that block, and will be checked if can be pushed.
	std::vector<unsigned> blockedColliders;
	std::vector<unsigned> blockingColliders;
	
	// First time calling the scan-for-blocking-and-pushable-objects function.
	ExpandMovementTree(dir, &pushingObjects, &objectsToPush, &pushingColliders, &collidersToPush, &blockedObjects, &blockingObjects, &blockedColliders, &blockingColliders);
	// After calling ExpandMovementTree, the vectors are populated with:
	// 1. The list of objects that are blocking.
	// 2. The list of objects that need pushing.

	// Start with objects that need pushing.
		// Here, some kind of recursion will occur, where objects try to move, might stumble on other objects, and will try to push whatever they can until nothing is left to push.
		// The hope here for the moving object is that there will be no blocking objects at the end of the recursion-loop.
	for (size_t i = 0; i < objectsToPush.size(); i++)
		objectsToPush[i]->ExpandMovementTree(dir, &pushingObjects, &objectsToPush, &pushingColliders, &collidersToPush, &blockedObjects, &blockingObjects, &blockedColliders, &blockingColliders);

	// No objects are blocking. They were never found OR were all pushed.
	if (blockingObjects.size() == 0)
	{
		// Move and call OnEvents
		for (size_t i = 0; i < objectsToPush.size(); i++)
		{
			objectsToPush[i]->pos.x += dir.x;
			objectsToPush[i]->pos.y += dir.y;
			objectsToPush[i]->lastPush = dir;
			objectsToPush[i]->theOtherObject = pushingObjects[i];
			objectsToPush[i]->theColliderIndex = collidersToPush[i];
			objectsToPush[i]->theOtherColliderIndex = pushingColliders[i];
			if (objectsToPush[i]->OnPushed)
				objectsToPush[i]->OnPushed();

			pushingObjects[i]->lastPush = dir;
			pushingObjects[i]->theOtherObject = objectsToPush[i];
			pushingObjects[i]->theColliderIndex = pushingColliders[i];
			pushingObjects[i]->theOtherColliderIndex = collidersToPush[i];
			if (pushingObjects[i]->OnPush)
				pushingObjects[i]->OnPush();
			// OnEvent functions are called only for objects that are pushed or blocking.
		}
		pos.x += dir.x;
		pos.y += dir.y;

		return true;
	}
	// Failed to move, blocked.
	// TODO: call OnBlock and OnBlocked functions before returning false.
	return false;
}

/*
Since an object has multiple colliders, things can't work like they worked before. And in addition, now there are 2 types
of colliders, simple and complex. Simple is a rectangle which is faster to process, and complex is a 2D bool array, which is slower
to process but allows complex shapes.

Idea:

Object is a gruop. If one of the colliders gets pushed, all of the other colliders in the group get pushed too.

If another collider is blocking the collider, the reference to the other collider should be the other object, becuase if any
of the other object's collider manage to get pushed, then the whole object moves.

So it is really just like an object moves with one collider, except the collider is split into different types, allowing
different positioning to affect the movement differently.


The first loop (Object::Move) is the loop that checks if any other objects are in the way of the object.
	If a blocking other object is found add the other collider (for proper OnEvent calling) and other object (for possible pushing). 
	If a pushable other object is found add the other collider and the other object to try push.
	If an overlappable other object is found, ignore.

	What's important here is that if we get to another object that got other colliders infront of the colliders,
		If one of the colliders can push any of the other colliders, then the other object will be pushed, memories it to try pushing it later.
		If non of the colliders can push any of the other colliders, then the other object is blocking, memories it to hope for another object that can push it.
	It's like the object is smart and searches for the best collider to push the other object.

At the end, there should be no blocking objects

Objects that are already in the pushing sequence should be ignored.


bool AreCollidersOverlapping(Collider* collider, Vector2D dir, Collider* otherCollider)

bool Move()
{
	std::vector<Collider*> blockingColliders;

	For (colliders)
	{
		For (other objects)
		{
			For (other_object.collider)
			{
				If (collisionResult == 2) // Overlapped
					continue;
				If (AreCollidersOverlapping(collider, dir, other_collider))
				{
					If (collisionResult == 0) // Blocked
					{
						blockingColliders.pushBack(other_collider);
					}
				}
			}
		}
	}
}

*/


/*
Bugs:
1:	Avoiding infinte push loop also means that when:
		 ABA
		#CB
	B moves to the left, A will move, but B won't, allowing A to overlap B:
		A A
		#CB
	Possible solution:
		The problem is that an object is pushing an object that will result in the first object getting pushed. Quite an unfortunate loop, but it would be quite nice if it worked.
		The problem rises from the probability that an object spawns overlapping upon another. If one of those objects move, and they can push each other, object A will push object B, which will realize it would need
		to push object A to move, which creates the infinite push loop. To fix it, I send a pointer to Move of object B of object A, telling it to ignore object A when moving.
		To fix it, we can check if object A and object B are overlapping from the first place and if not, don't send the pointer. But -
		| Good I written all of this useless text above, because the point I was getting to is totally irrelevant. The solution lays bellow bug 2. I gave a wrong example, where C was a block.
		B can later go through A, don't worry, that was an intentional desicion you made to let objects that overlap (by accident or not) go through each other to free themselves.
	SOLVED:
		Added system mentioned in bug #2. More details in solution #2

2:	When pushing objects, can push one object, try to push another but this time the object can't be pushed, allowing:
		AB
		AC#
	A moves to the right, B will be pushed, C won't, A won't move:
		A B
		AC#
	Possible solution:
		This means a huge tree of checking again and again if all objects will be able to be at a certan position. What if B is also infront of 2 objects, where one is blocked and the other isn't?
		The problem with this system is to make it perfect, it can possibly require a sheer amount of iterations.
		But, there could be a way to make a tree that grows only the amount needed, it just needs quite a bit of planning, and if it works, there will be potentially no more problems, because all of them rise from this.
	SOLVED:
		If you tried you could possibly make a huge tree that will be seen in the FPS counter, but it completely works now, bugs #1 and #2 are now non-existent. What I had to do to make this tree-system is actually pretty simple:
		I made a function called "CanMove", which does what Move does but without moving and calling user defined functions (like OnCollision). Diagram kinda explains it:
		Move (User called) -> CanMove (Move called) for all objects to push
							  |
							  V
							  Sends vector of pushing objects, currently only original object is there.																	
							  |																																			
							  | <-----------------------------------------------------------------------------------------------------------------------------------------\
							  V																																		      |
							  Has objects in the way? -> Yes -> Can push? -> Yes -> Call CanMove on objects to move, with the tree vector added the other pushing object -/
							  |									|
							  V									V
							  No								No -> Return false, ending the tree, can't move. -> Don't move, things are on the way which can't be pushed. END
							  |
							  V
							  Return true (can move), ending or continuing CanPush tree. -> Ended the tree? -> Push all objects to push (Move object, called by Move which was called by user). END

3:	When 2 objects that were added early push an object that was added late, both objects will push it at the same time resulting the late object to be double pushed.
		BAC
		BBC
	B pushes A, resulting in both pushing C. C was added later.
		 BA C
		 BB C
	Possible solution:
		Check before pushing if the object was already pushed. Probably recheck collision which will deal another massive down in performance.

		The current problem is that we are pushing objects we already pushed. Perhaps we need to collect all objects to push, or something similar.
		Currently, we make a tree of objects, check if they can move, if they can't, move nothing. If everything can move, move it all. But we forget the tree while constructing it. Perhaps all we need is to memorize it,
		eliminate duplicates, and move. Perhaps there is even a better way - check if all can move, preorganize function calls, then change position without recalculating everything - a smarter system, that will in advance be fast.

		Move -> If nothing blocks -> For (objects to push)
									 |
									 V *With ignore list of pushing objects
									 Can Move -> Collect tree of objects, detailing what collided what to later call OnCollision and OnPushed -> Blocked? -> Yes -> Move nothing, add current object and blocking object to tree.
																																				 |
																																				 V
																																				 No
																																				 |
																																				 V
																																			---- Pushable? -> Yes -> For (objects to push) -> ...
																																			|	 |
																																			V	 V
																															Last in the tree? <- Nothing in the way? -> Return up in the tree, add current object to pushable.
																															|
																															V
																															For (tree)
																															|
																															V
	SOLVED
		blockmoving is now the new way to move multiple objects at the same time. It works great and it's bugless (so far).

		  %%
		@@%%^^
		@@##^^
		@@##

		@ @ %
		% # ^

		  ##
		######
		######
		####
*/
