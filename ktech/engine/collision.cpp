/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2024 Ethan Kaufman (AKA Kaup)

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

#include "collision.hpp"

#include "../basic/point.hpp"
#include "../basic/upoint.hpp"
#include "../world/layer.hpp"
#include "../world/object.hpp"
#include "../engine/engine.hpp"

#include <algorithm>

#define OBJECTS engine.memory.objects
#define LAYERS engine.memory.layers

/*!
	@var `Collision::CR`

	Collision result.

	There are 3 possible collision results: `CR::B` (block), `CR::P` (push), and `CR::O` (overlap).

	@see `Collision::colliderTypes`
*/
/*!
	@var `Collision::colliderTypes`

	Matrix representing collider types and their potential collision results.

	An `Object`'s `Collider` (each of `Object::m_colliders`) can have a distinct "collider type" (`Collider::m_type`). Collider types are defined in this 2D vector.

	The index of each row or column is a collider type. The intersection between 2 collider types is their collision result (`Collision::CR`) if the row type was of the moving collider, and the column type was of the passive collider.

	For example, here's how you should look at the default values:

	| What if (row ↓) collides with (column →) | Type 0  | Type 1  | Type 2  |
	|------------------------------------------|---------|---------|---------|
	| Type 0                                   | Block   | Push    | Overlap |
	| Type 1                                   | Block   | Push    | Overlap |
	| Type 2                                   | Overlap | Overlap | Overlap |

	Meaning, colliders of type 0 can't be pushed, colliders of type 1 can be pushed, and colliders of type 2 overlap with everything.

	You can set this variable to fit your needs. This matrix should be a square so all available collider types have a defined collision result between them. If a row or a column is missing, the default collision result is `CR::O` (overlap).

	@see `Collision::CR`
	@see `Collider::m_type`
*/

/*!
	Move an `Object` in relation to other `Object`s from the same `Layer`.

	`Object::Move()` calls this function on itself.

	This function processes all `Object`s in the `Layer` to judge whether the moving `Object` can move, or there are blocking `Collider`s. It creates a "movement tree", allowing `Object`s to be pushed one after another in a sequence. This function tries to form an optimal movement tree: it won't stop looking for a way to push all discovered blocking `Object`s, until there are no more `Object`s left to process (or a way was found).

	Based on the final result, it can call back the following virtual `Object` functions (with the appropriate parameters):

	- `Object::OnMove()` for `Object`s that moved (voluntarily or passively).
	- `Object::OnPushed()` for `Object`s that were pushed by another `Object`.
	- `Object::OnPush()` for `Object`s that pushed another `Object`.
	- `Object::OnBlocked()` for `Object`s that were blocked by another `Object`.
	- `Object::OnBlock()` for `Object`s that blocked another `Object`.
	- `Object::OnOverlap()` for `Object`s that overlapped into another `Object`.
	- `Object::OnOverlapExit()` for `Object`s that left from overlapping with another `Object`.
	- `Object::OnOverlapped()` for `Object`s that were overlapped into by another `Object`.
	- `Object::OnOverlappedExit()` for `Object`s that were left from overlapping by another `Object`.

	@see `Object`
*/
auto KTech::Collision::MoveObject(ID<Object>& p_object, Point p_direction) -> bool
{
	std::vector<CollisionData> pushData;
	std::vector<CollisionData> blockData;
	std::vector<CollisionData> overlapData;
	std::vector<CollisionData> exitOverlapData;

	// Start root of movement trees
	ExpandMovementTree(p_object, p_direction, pushData, blockData, overlapData, exitOverlapData);
	// Expand until no more objects in the area can be pushed
	for (size_t i = 0; i < pushData.size(); i++)
	{
		ExpandMovementTree(pushData[i].passiveObject, p_direction, pushData, blockData, overlapData, exitOverlapData);
	}

	// Able to move - no blocks at the end (could be that no blocking objects were found or all blocking objects were found to be pushable)
	if (blockData.empty())
	{
		// Change positions
		for (const CollisionData& pushDatum : pushData)
		{
			OBJECTS[pushDatum.passiveObject]->m_pos.x += p_direction.x;
			OBJECTS[pushDatum.passiveObject]->m_pos.y += p_direction.y;
		}
		OBJECTS[p_object]->m_pos.x += p_direction.x;
		OBJECTS[p_object]->m_pos.y += p_direction.y;
		// Call push events
		for (const CollisionData& pushDatum : pushData)
		{
			OBJECTS[pushDatum.activeObject]->OnPush(p_direction, pushDatum.activeCollider, pushDatum.passiveObject, pushDatum.passiveCollider);
			OBJECTS[pushDatum.passiveObject]->OnPushed(p_direction, pushDatum.passiveCollider, pushDatum.activeObject, pushDatum.activeCollider);
			OBJECTS[pushDatum.passiveObject]->OnMove(p_direction);
		}
		// Call overlap events
		for (const CollisionData& overlapDatum : overlapData)
		{
			OBJECTS[overlapDatum.activeObject]->OnOverlap(p_direction, overlapDatum.activeCollider, overlapDatum.passiveObject, overlapDatum.passiveCollider);
			OBJECTS[overlapDatum.passiveObject]->OnOverlapped(p_direction, overlapDatum.passiveCollider, overlapDatum.activeObject, overlapDatum.activeCollider);
		}
		// Call overlap exit events
		for (const CollisionData& exitOverlapDatum : exitOverlapData)
		{
			OBJECTS[exitOverlapDatum.activeObject]->OnOverlapExit(p_direction, exitOverlapDatum.activeCollider, exitOverlapDatum.passiveObject, exitOverlapDatum.passiveCollider);
			OBJECTS[exitOverlapDatum.passiveObject]->OnOverlappedExit(p_direction, exitOverlapDatum.passiveCollider, exitOverlapDatum.activeObject, exitOverlapDatum.activeCollider);
		}
		OBJECTS[p_object]->OnMove(p_direction);
		return true;
	}
	// Unable to move - there are blocking objects.
	for (const CollisionData& blockDatum : blockData)
	{
		OBJECTS[blockDatum.activeObject]->OnBlocked(p_direction, blockDatum.activeCollider, blockDatum.passiveObject, blockDatum.passiveCollider);
		OBJECTS[blockDatum.passiveObject]->OnBlock(p_direction, blockDatum.passiveCollider, blockDatum.activeObject, blockDatum.activeCollider);
	}
	return false;
}

auto KTech::Collision::GetPotentialCollisionResult(uint8_t p_type1, uint8_t p_type2) -> CR
{
	CR result = CR::O;
	if (p_type1 < colliderTypes.size())
	{
		if (p_type2 < colliderTypes[p_type1].size())
		{
			result = colliderTypes[p_type1][p_type2];
		}
	}
	return result;
}

auto KTech::Collision::AreCollidersOverlapping(const Collider& p_collider1, const Point& p_position1, const Collider& p_collider2, const Point& p_position2) -> bool
{
	// First check for simple overlap
	if (AreSimpleCollidersOverlapping(p_collider1, p_position1, p_collider2, p_position2))
	{
		if (p_collider1.m_simple)
		{
			if (p_collider2.m_simple)
			{
				// Both are simple
				return true;
			}
			// `p_collider1` is simple, `p_collider2` is complex
			return AreSimpleAndComplexCollidersOverlapping(p_collider2, p_position2, p_collider1, p_position1);
		}
		if (p_collider2.m_simple)
		{
			// `p_collider1` is complex, `p_collider2` is simple
			return AreSimpleAndComplexCollidersOverlapping(p_collider1, p_position1, p_collider2, p_position2);
		}
		// Both are complex
		return AreComplexCollidersOverlapping(p_collider1, p_position1, p_collider2, p_position2);
	}
	// If there's not even simple overlap, there will be no complex overlap as well
	return false;
}

auto KTech::Collision::AreSimpleCollidersOverlapping(const Collider& p_collider1, const Point& p_position1, const Collider& p_collider2, const Point& p_position2) -> bool
{
	return p_position1.x < p_position2.x + p_collider2.m_size.x &&
		p_position1.x + p_collider1.m_size.x > p_position2.x &&
		p_position1.y < p_position2.y + p_collider2.m_size.y &&
		p_position1.y + p_collider1.m_size.y > p_position2.y;
}

auto KTech::Collision::AreSimpleAndComplexCollidersOverlapping(const Collider& p_complex, const Point& p_complexPosition, const Collider& p_simple, const Point& p_simplePosition) -> bool
{
	for (size_t y = (p_complexPosition.y < p_simplePosition.y ? p_simplePosition.y - p_complexPosition.y : 0); y < p_complex.m_size.x && p_complexPosition.y + y < p_simplePosition.y + p_simple.m_size.y; y++)
	{
		for (size_t x = (p_complexPosition.x < p_simplePosition.x ? p_simplePosition.x - p_complexPosition.x : 0); x < p_complex.m_size.x && p_complexPosition.x + x < p_simplePosition.x + p_simple.m_size.x; x++)
		{
			if (p_complex(x, y))
			{
				return true;
			}
		}
	}
	return false;
}

auto KTech::Collision::AreComplexCollidersOverlapping(const Collider& p_collider1, const Point& p_position1, const Collider& p_collider2, const Point& p_position2) -> bool
{
	// `p_collider1` and `p_collider2` are complex
	size_t y1 = 0;
	size_t y2 = 0;
	// If complex1 is before complex2, then add to y1 so it starts where complex2 starts, and keep y2 0
	if (p_position1.y < p_position2.y)
	{
		y1 = p_position2.y - p_position1.y;
	}
	// Otherwise, if complex2 is before complex1, then add to y2 so it starts where complex1 starts, and keep y1 0
	else if (p_position2.y < p_position1.y)
	{
		y2 = p_position1.y - p_position2.y;
	}

	// Iterate with both Ys until one of them reaches the end of their colliders
	for (; y1 < p_collider1.m_size.y && y2 < p_collider2.m_size.y; y1++, y2++)
	{
		size_t x1 = 0;
		size_t x2 = 0;
		// If complex1 is before complex2, then add to x1 so it starts where complex2 starts, and keep x2 0
		if (p_position1.x < p_position2.x)
		{
			x1 = p_position2.x - p_position1.x;
		}
		// Otherwise, if complex2 is before complex1, then add to x2 so it starts where complex1 starts, and keep x1 0
		else if (p_position2.x < p_position1.x)
		{
			x2 = p_position1.x - p_position2.x;
		}

		// Iterate with both Xs until one of them reaches the end of their colliders
		for (; x1 < p_collider1.m_size.x && x2 < p_collider2.m_size.x; x1++, x2++)
		{
			if (p_collider1(x1, y1) && p_collider2(x2, y2))
			{
				return true;
			}
		}
	}
	return false;
}

void KTech::Collision::ExpandMovementTree(ID<Object>& p_thisObject, Point p_direction,
	std::vector<CollisionData>& p_pushData,
	std::vector<CollisionData>& p_blockData,
	std::vector<CollisionData>& p_overlapData,
	std::vector<CollisionData>& p_exitOverlapData)
{
	for (ID<Object>& otherObject : LAYERS[OBJECTS[p_thisObject]->m_parentLayer]->m_objects) // Other objects
	{
		if (otherObject == p_thisObject
			|| IsInPushData(p_pushData, otherObject)) // Filter out pushed/pushing objects (leaving objects outside the movement tree and objects from `blockingObjects`)
		{
			continue;
		}

		// *Needed in this scope!
		CR collisionResult = CR::O; // The collision result between this object and this other object. Default - overlap (nothing, ignored).
		size_t colliderI; // Collider index
		size_t otherColliderI; // Other collider index
		size_t originallyBlockedColliderI; // Used if not found a pushable collider
		size_t originallyBlockingColliderI; // ^

		for (colliderI = 0; colliderI < OBJECTS[p_thisObject]->m_colliders.size(); colliderI++) // This object's colliders
		{
			// Set collider
	 		const Collider& col = (OBJECTS[p_thisObject]->m_colliders[colliderI]);
			// Skip this one if it is invalid
			if (!col.m_active || col.m_size.x == 0 || col.m_size.y == 0)
			{
				continue;
			}

			for (otherColliderI = 0; otherColliderI < OBJECTS[otherObject]->m_colliders.size(); otherColliderI++) // Other object's colliders
			{
				// Set other collider
				const Collider& otherCol = OBJECTS[otherObject]->m_colliders[otherColliderI];
				// Skip this one if it is invalid
				if (!otherCol.m_active || otherCol.m_size.x == 0 || otherCol.m_size.y == 0)
				{
					continue;
				}

				// Get the potential collision result in advance
				CR potentialCollisionResult = GetPotentialCollisionResult(col.m_type, otherCol.m_type);

				// If the current known result is block (0) and the potential result of this other object is also block then there is no reason to proceed checking.
				// If the current known result if push (1) and the potential result is also push then once again there is no reason to proceed checking.
				// But, if the potential result is overlap then proceed checking no matter the known result, we want to get all the overlap occurrences.

				// Check enter/exit overlap events
				if (potentialCollisionResult == CR::O)
				{
					// Compare current and future overlapping according to the types of the colliders
					bool currentOverlapState = AreCollidersOverlapping(col, OBJECTS[p_thisObject]->m_pos + col.m_rPos, otherCol, OBJECTS[otherObject]->m_pos + otherCol.m_rPos);
					bool futureOverlapState = AreCollidersOverlapping(col, OBJECTS[p_thisObject]->m_pos + col.m_rPos + p_direction, otherCol, OBJECTS[otherObject]->m_pos);

					if (!currentOverlapState && futureOverlapState)
					{
						p_overlapData.push_back({p_thisObject, otherObject, colliderI, otherColliderI}); // Entered overlap
					}
					else if (currentOverlapState && !futureOverlapState)
					{
						p_exitOverlapData.push_back({p_thisObject, otherObject, colliderI, otherColliderI}); // Exited overlap
					}
				}
				// Proceed to checking collision (repositioned overlap) only if the potential collision result is not already known.
				// This means that if the currently known CR is a block and the potential CR is also a block, then there is no reason to check collision again since encountering another blocking collider doesn't matter.
				// And, if the currently known CR is a push, then there is no reason to check for blocks or more ways to push, since the other object is going to get pushed either way.
				// But, if the currently known CR is a push, the for loops should still keep going in order to check for more overlaps, which are always relevant.
				// Basically, by detecting a block, this part of the code gets locked on a block, until a push is found, which causes this part of the code to get locked on a push and never check for more collisions again in the current `otherObj`'s iteration.
				else if (collisionResult != potentialCollisionResult)
				{
					if (AreCollidersOverlapping(col, OBJECTS[p_thisObject]->m_pos + col.m_rPos + p_direction, otherCol, OBJECTS[otherObject]->m_pos + otherCol.m_rPos))
					{
						// Collision!
						collisionResult = potentialCollisionResult;
						if (collisionResult == CR::B)
						{
							// There can only be one time (for each other object) where a block result can get to this point in the current `otherObj`'s iteration.
							// That means that `c` and `oc` represent the original blocked and blocking colliders, which can be used for user-defined functions calling.
							originallyBlockedColliderI = colliderI;
							originallyBlockingColliderI = otherColliderI;
						}
					}
				}
			}
		}

		if (collisionResult == CR::B)
		{
			p_blockData.push_back({p_thisObject, otherObject, originallyBlockedColliderI, originallyBlockingColliderI});
		}
		else if (collisionResult == CR::P)
		{
			p_pushData.push_back({p_thisObject, otherObject, otherColliderI, colliderI});
		}
	}
}

auto KTech::Collision::IsInPushData(std::vector<CollisionData>& p_pushData, ID<Object>& p_object) -> bool
{
    return std::ranges::any_of(p_pushData,
		[&p_object](const CollisionData& pushDatum)
		{
        	return p_object == pushDatum.activeObject || p_object == pushDatum.passiveObject;
    	}
	);
}