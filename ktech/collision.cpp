/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 E. Kaufman (AKA Kaup)

	This file is part of KTech.

	KTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	KTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with KTech. If not, see <https://www.gnu.org/licenses/>.
*/

#include "ktech.hpp"

using namespace KTech;

CR Collision::GetPotentialCollisionResult(uint8_t t1, uint8_t t2)
{
	CR result = CR::O;
	if (t1 >= 0 && t1 < colliderTypes.size())
		if (t2 >= 0 && t2 < colliderTypes[t1].size())
			result = colliderTypes[t1][t2];
	return result;
}

// `s` - simple (rectangle)
// `p` - global position
bool Collision::AreSimpleCollidersOverlapping(UPoint s1, Point p1, UPoint s2, Point p2)
{
 	return (p1.x < p2.x + s2.x && p1.x + s1.x > p2.x && p1.y < p2.y + s2.y && p1.y + s1.y > p2.y);
}
// `s` - simple (rectangle)
// `c` - complex (2d vector)
// `p` - global position
bool Collision::AreSimpleAndComplexCollidersOverlapping(UPoint simple, Point simplePos, const std::vector<std::vector<bool>>& complex, Point complexPos)
{
	// Start Y from 0 if the simple is before the complex, or at the same position.
	// Otherwise, add some to Y so the scanning will start from the point in which the simple collider starts.
	size_t y = (complexPos.y < simplePos.y ? simplePos.y - complexPos.y : 0);
	// Iterate with Y until it reaches the end of the complex, or the end of the simple.
	for (; y < complex.size() && complexPos.y + y < simplePos.y + simple.y; y++)
	{
		// Start X from 0 if the simple is before the complex, or at the same position.
		// Otherwise, add some to X so the scanning will start from the point in which the simple collider starts.
		size_t x = (complexPos.x < simplePos.x ? simplePos.x - complexPos.x : 0);
		// Iterate with X until it reaches the end of the complex, or the end of the simple.
		for (; x < complex[y].size() && complexPos.x + x < simplePos.x + simple.x; x++)
			if (complex[y][x])
				if (simplePos.x <= complexPos.x + x && complexPos.x + x <= simplePos.x + simple.x && simplePos.y <= complexPos.y + y && complexPos.y + y <= simplePos.y + simple.y)
					return true;
	}
	return false;
}

// `c` - complex (2d vector)
// `p` - global position
bool Collision::AreComplexCollidersOverlapping(const std::vector<std::vector<bool>>& c1, Point p1, const std::vector<std::vector<bool>>& c2, Point p2)
{
	size_t y1 = 0;
	size_t y2 = 0;
	// If complex1 is before complex2, then add to y1 so it starts where complex2 starts, and keep y2 0
	if (p1.y < p2.y)
		y1 = p2.y - p1.y;
	// Otherwise, if complex2 is before complex1, then add to y2 so it starts where complex1 starts, and keep y1 0
	else if (p2.y < p1.y)
		y2 = p1.y - p2.y;

	// Iterate with both Ys until one of them reaches the end of their colliders
	for (; y1 < c1.size() && y2 < c2.size(); y1++, y2++)
	{
		size_t x1 = 0;
		size_t x2 = 0;
		// If complex1 is before complex2, then add to x1 so it starts where complex2 starts, and keep x2 0
		if (p1.x < p2.x)
			x1 = p2.x - p1.x;
		// Otherwise, if complex2 is before complex1, then add to x2 so it starts where complex1 starts, and keep x1 0
		else if (p2.x < p1.x)
			x2 = p1.x - p2.x;

		// Iterate with both Xs until one of them reaches the end of their colliders
		for (; x1 < c1[y1].size() && x2 < c2[y2].size(); x1++, x2++)
			if (c1[y1][x1] && c2[y2][x2])
				return true;
	}
	return false;
}

void Collision::ExpandMovementTree(Object* thisObj, Point dir,
	std::vector<CollisionData>& pushData,
	std::vector<CollisionData>& blockData,
	std::vector<CollisionData>& overlapData,
	std::vector<CollisionData>& exitOverlapData)
{
	static Object* otherObject;
	static Collider* collider;
	static Collider* otherCollider;
	static bool currentOverlapState, futureOverlapState;

	for (size_t o = 0; o < thisObj->parentLayer->objects.size(); o++)
	{
		otherObject = thisObj->parentLayer->objects[o];

		// Filter in only objects which aren't a part of the movement tree and objects from `blockingObjects`
		for (size_t i = 0; i < pushData.size(); i++) {
			if (otherObject == pushData[i].activeObject || otherObject == pushData[i].passiveObject) {
				otherObject = thisObj; // Stupid way to exit the other for-loop but whatever.
			}
		}
		if (otherObject == thisObj)
			continue;
		
		CR cr = CR::O; // The collision result between this object and this other object. Default - overlap (nothing, ignored).
		size_t c = 0; // Collider index (needed after for loops)
		size_t oc = 0; // Other collider index (needed after for loops)
		size_t originallyBlockedColliderI = 0; // Used if not found a pushable collider
		size_t originallyBlockingColliderI = 0; // ^

		for (size_t c = 0; c < thisObj->colliders.size(); c++)
		{
			// Set collider
	 		collider = &thisObj->colliders[c];
			// Skip this one if it is invalid
			if (!collider->active || collider->GetSize().x == 0 || collider->GetSize().y == 0)
				continue;

			for (oc = 0; oc < otherObject->colliders.size(); oc++)
			{
				// Set other collider
				otherCollider = &otherObject->colliders[oc];
				// Skip this one if it is invalid
				if (!otherCollider->active || otherCollider->GetSize().x == 0 || otherCollider->GetSize().y == 0)
					continue;

				// Get the potential collision result in advance
				CR potentialCollisionResult = GetPotentialCollisionResult(collider->type, otherCollider->type);

				// If the current known result is block (0) and the potential result of this other object is also block then there is no reason to proceed checking.
				// If the current known result if push (1) and the potential result is also push then once again there is no reason to proceed checking.
				// But, if the potential result is overlap then proceed checking no matter the known result, we want to get all the overlaps occurences.
				
				// Check enter/exit overlap events
				if (potentialCollisionResult == CR::O)
				{
					// Compare current and future overlapping according to the types of the colliders
					if (collider->simple)
					{
						if (otherCollider->simple)
						{
							currentOverlapState = AreSimpleCollidersOverlapping(collider->size, Point(collider->pos_r.x + thisObj->pos.x, collider->pos_r.y + thisObj->pos.y), otherCollider->size, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
							futureOverlapState = AreSimpleCollidersOverlapping(collider->size, Point(collider->pos_r.x + thisObj->pos.x + dir.x, collider->pos_r.y + thisObj->pos.y + dir.y), otherCollider->size, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
						}
						else
						{
							currentOverlapState = AreSimpleAndComplexCollidersOverlapping(collider->size, Point(collider->pos_r.x + thisObj->pos.x, collider->pos_r.y + thisObj->pos.y), otherCollider->c, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
							futureOverlapState = AreSimpleAndComplexCollidersOverlapping(collider->size, Point(collider->pos_r.x + thisObj->pos.x + dir.x, collider->pos_r.y + thisObj->pos.y + dir.y), otherCollider->c, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
						}
					}
					else
					{
						if (otherCollider->simple)
						{
							currentOverlapState = AreSimpleAndComplexCollidersOverlapping(otherCollider->size, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y), collider->c, Point(collider->pos_r.x + thisObj->pos.x, collider->pos_r.y + thisObj->pos.y));
							futureOverlapState = AreSimpleAndComplexCollidersOverlapping(otherCollider->size, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y), collider->c, Point(collider->pos_r.x + thisObj->pos.x + dir.x, collider->pos_r.y + thisObj->pos.y + dir.y));
						}
						else
						{
							currentOverlapState = AreComplexCollidersOverlapping(collider->c, Point(collider->pos_r.x + thisObj->pos.x, collider->pos_r.y + thisObj->pos.y), otherCollider->c, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
							futureOverlapState = AreComplexCollidersOverlapping(collider->c, Point(collider->pos_r.x + thisObj->pos.x + dir.x, collider->pos_r.y + thisObj->pos.y + dir.y), otherCollider->c, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
						}
					}

					if (!currentOverlapState && futureOverlapState)
						overlapData.push_back({thisObj, otherObject, c, oc}); // Entered overlap!
					else if (currentOverlapState && !futureOverlapState)
						exitOverlapData.push_back({thisObj, otherObject, c, oc}); // Exited overlap!
				}
				// Proceed to checking collision (repositioned overlap) only if the potential collision result is not already known.
				// This means that if the currently known CR is a block and the potential CR is also a block, then there is no reason to check collision again since encountering another blocking collider doesn't matter.
				// And, if the currently known CR is a push, then there is no reason to check for blocks or more ways to push, since the other object is going to get pushed either way.
				// But, if the currently known CR is a push, the for loops should still keep going in order to check for more overlaps, which are always relevant.
				// Basically, by detecting a block, this part of the code gets locked on a block, until a push is found, which causes this part of the code to get locked on a push and never check for more collisions again in the current `otherObject`'s iteration.
				else if (cr != potentialCollisionResult)
				{
					// Compare future overlapping according to the types of the colliders
					if (collider->simple)
					{
						if (otherCollider->simple)
							futureOverlapState = AreSimpleCollidersOverlapping(collider->size, Point(collider->pos_r.x + thisObj->pos.x + dir.x, collider->pos_r.y + thisObj->pos.y + dir.y), otherCollider->size, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
						else
							futureOverlapState = AreSimpleAndComplexCollidersOverlapping(collider->size, Point(collider->pos_r.x + thisObj->pos.x + dir.x, collider->pos_r.y + thisObj->pos.y + dir.y), otherCollider->c, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
					}
					else
					{
						if (otherCollider->simple)
							futureOverlapState = AreSimpleAndComplexCollidersOverlapping(otherCollider->size, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y), collider->c, Point(collider->pos_r.x + thisObj->pos.x + dir.x, collider->pos_r.y + thisObj->pos.y + dir.y));
						else
							futureOverlapState = AreComplexCollidersOverlapping(collider->c, Point(collider->pos_r.x + thisObj->pos.x + dir.x, collider->pos_r.y + thisObj->pos.y + dir.y), otherCollider->c, Point(otherCollider->pos_r.x + otherObject->pos.x, otherCollider->pos_r.y + otherObject->pos.y));
					}

					if (futureOverlapState)
					{
						// Collision!
						cr = potentialCollisionResult;
						if (cr == CR::B)
						{
							// There can only be one time (for each other object) where a block result can get to this point in the current `otherObject`'s iteration.
							// That means that `c` and `oc` represent the original blocked and blocking colliders, which can be used for user-defined functions calling.
							originallyBlockedColliderI = c;
							originallyBlockingColliderI = oc;
						}
					}
				}
			}
	}

		if (cr == CR::B)
			blockData.push_back({thisObj, otherObject, originallyBlockedColliderI, originallyBlockingColliderI});
		else if (cr == CR::P)
			pushData.push_back({thisObj, otherObject, oc, c});
	}
}

bool Collision::MoveObject(Object* object, Point dir)
{
	std::vector<CollisionData> pushData;
	std::vector<CollisionData> blockData;
	std::vector<CollisionData> overlapData;
	std::vector<CollisionData> exitOverlapData;
	
	// Start root of movement trees
	ExpandMovementTree(object, dir, pushData, blockData, overlapData, exitOverlapData);
	// Expand until no more objects in the area can be pushed
	for (size_t i = 0; i < pushData.size(); i++)
		ExpandMovementTree(pushData[i].passiveObject, dir, pushData, blockData, overlapData, exitOverlapData);

	// Able to move - no blocks at the end (could be that no blocking objects were found or all blocking objects were found to be pushable)
	if (blockData.size() == 0)
	{
		// Change positions
		for (size_t i = 0; i < pushData.size(); i++)
		{
			pushData[i].passiveObject->pos.x += dir.x;
			pushData[i].passiveObject->pos.y += dir.y;
		}
		object->pos.x += dir.x;
		object->pos.y += dir.y;
		// Call push events
		for (size_t i = 0; i < pushData.size(); i++)
		{
			pushData[i].activeObject->lastPush = dir;
			pushData[i].activeObject->colliderIndex = pushData[i].activeCollider;
			pushData[i].activeObject->otherObject = pushData[i].passiveObject;
			pushData[i].activeObject->otherColliderIndex = pushData[i].passiveCollider;

			pushData[i].passiveObject->lastPush = dir;
			pushData[i].passiveObject->colliderIndex = pushData[i].passiveCollider;
			pushData[i].passiveObject->otherObject = pushData[i].activeObject;
			pushData[i].passiveObject->otherColliderIndex = pushData[i].activeCollider;
			
			pushData[i].activeObject->OnEvent(Object::EventType::onPush);
			pushData[i].passiveObject->OnEvent(Object::EventType::onPushed);
		}
		// Call overlap events
		for (size_t i = 0; i < overlapData.size(); i++)
		{
			overlapData[i].activeObject->lastPush = dir;
			overlapData[i].activeObject->colliderIndex = overlapData[i].activeCollider;
			overlapData[i].activeObject->otherObject = overlapData[i].passiveObject;
			overlapData[i].activeObject->otherColliderIndex = overlapData[i].passiveCollider;

			overlapData[i].passiveObject->lastPush = dir;
			overlapData[i].passiveObject->colliderIndex = overlapData[i].passiveCollider;
			overlapData[i].passiveObject->otherObject = overlapData[i].activeObject;
			overlapData[i].passiveObject->otherColliderIndex = overlapData[i].activeCollider;

			overlapData[i].activeObject->OnEvent(Object::EventType::onOverlap);
			overlapData[i].passiveObject->OnEvent(Object::EventType::onOverlapped);
		}
		// Call overlap exit events
		for (size_t i = 0; i < exitOverlapData.size(); i++)
		{
			exitOverlapData[i].activeObject->lastPush = dir;
			exitOverlapData[i].activeObject->colliderIndex = exitOverlapData[i].activeCollider;
			exitOverlapData[i].activeObject->otherObject = exitOverlapData[i].passiveObject;
			exitOverlapData[i].activeObject->otherColliderIndex = exitOverlapData[i].passiveCollider;

			exitOverlapData[i].passiveObject->lastPush = dir;
			exitOverlapData[i].passiveObject->colliderIndex = exitOverlapData[i].passiveCollider;
			exitOverlapData[i].passiveObject->otherObject = exitOverlapData[i].activeObject;
			exitOverlapData[i].passiveObject->otherColliderIndex = exitOverlapData[i].activeCollider;

			exitOverlapData[i].activeObject->OnEvent(Object::EventType::onOverlapExit);
			exitOverlapData[i].passiveObject->OnEvent(Object::EventType::onOverlappedExit);
		}
		return true;
	}
	// Unable to move - there are blocking objects.
	else
	{
		Object* theOtherObject = NULL;
		Point lastPush = { 0, 0 };
		int theColliderIndex = -1;
		int theOtherColliderIndex = -1;
		for (size_t i = 0; i < blockData.size(); i++)
		{
			blockData[i].activeObject->lastPush = dir;
			blockData[i].activeObject->colliderIndex = blockData[i].activeCollider;
			blockData[i].activeObject->otherObject = blockData[i].passiveObject;
			blockData[i].activeObject->otherColliderIndex = blockData[i].passiveCollider;

			blockData[i].passiveObject->lastPush = dir;
			blockData[i].passiveObject->colliderIndex = blockData[i].passiveCollider;
			blockData[i].passiveObject->otherObject = blockData[i].activeObject;
			blockData[i].passiveObject->otherColliderIndex = blockData[i].activeCollider;

			blockData[i].activeObject->OnEvent(Object::EventType::onBlocked);
			blockData[i].passiveObject->OnEvent(Object::EventType::onBlock);
		}
		return false; 
	}
}