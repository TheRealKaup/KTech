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

#define OBJECTS engine->memory.objects
#define LAYERS engine->memory.layers

bool KTech::Collision::MoveObject(ID<Object>& p_obj, Point p_dir)
{
	std::vector<CollisionData> pushData;
	std::vector<CollisionData> blockData;
	std::vector<CollisionData> overlapData;
	std::vector<CollisionData> exitOverlapData;

	// Start root of movement trees
	ExpandMovementTree(p_obj, p_dir, pushData, blockData, overlapData, exitOverlapData);
	// Expand until no more objects in the area can be pushed
	for (size_t i = 0; i < pushData.size(); i++)
		ExpandMovementTree(pushData[i].passiveObject, p_dir, pushData, blockData, overlapData, exitOverlapData);

	// Able to move - no blocks at the end (could be that no blocking objects were found or all blocking objects were found to be pushable)
	if (blockData.size() == 0)
	{
		// Change positions
		for (size_t i = 0; i < pushData.size(); i++)
		{
			OBJECTS[pushData[i].passiveObject]->m_pos.x += p_dir.x;
			OBJECTS[pushData[i].passiveObject]->m_pos.y += p_dir.y;
		}
		OBJECTS[p_obj]->m_pos.x += p_dir.x;
		OBJECTS[p_obj]->m_pos.y += p_dir.y;
		// Call push events
		for (size_t i = 0; i < pushData.size(); i++)
		{
			OBJECTS[pushData[i].activeObject]->OnPush(p_dir, pushData[i].activeCollider, pushData[i].passiveObject, pushData[i].passiveCollider);
			OBJECTS[pushData[i].passiveObject]->OnPushed(p_dir, pushData[i].passiveCollider, pushData[i].activeObject, pushData[i].activeCollider);
		}
		// Call overlap events
		for (size_t i = 0; i < overlapData.size(); i++)
		{
			OBJECTS[overlapData[i].activeObject]->OnOverlap(p_dir, overlapData[i].activeCollider, overlapData[i].passiveObject, overlapData[i].passiveCollider);
			OBJECTS[overlapData[i].passiveObject]->OnOverlapped(p_dir, overlapData[i].passiveCollider, overlapData[i].activeObject, overlapData[i].activeCollider);
		}
		// Call overlap exit events
		for (size_t i = 0; i < exitOverlapData.size(); i++)
		{
			OBJECTS[exitOverlapData[i].activeObject]->OnOverlapExit(p_dir, exitOverlapData[i].activeCollider, exitOverlapData[i].passiveObject, exitOverlapData[i].passiveCollider);
			OBJECTS[exitOverlapData[i].passiveObject]->OnOverlappedExit(p_dir, exitOverlapData[i].passiveCollider, exitOverlapData[i].activeObject, exitOverlapData[i].activeCollider);
		}
		return true;
	}
	// Unable to move - there are blocking objects.
	else
	{
		for (size_t i = 0; i < blockData.size(); i++)
		{
			OBJECTS[blockData[i].activeObject]->OnBlocked(p_dir, blockData[i].activeCollider, blockData[i].passiveObject, blockData[i].passiveCollider);
			OBJECTS[blockData[i].passiveObject]->OnBlock(p_dir, blockData[i].passiveCollider, blockData[i].activeObject, blockData[i].activeCollider);
		}
		return false; 
	}
}

KTech::CR KTech::Collision::GetPotentialCollisionResult(uint8_t p_t1, uint8_t p_t2)
{
	CR result = CR::O;
	if (p_t1 < colliderTypes.size())
		if (p_t2 < colliderTypes[p_t1].size())
			result = colliderTypes[p_t1][p_t2];
	return result;
}

bool KTech::Collision::AreCollidersOverlapping(const Collider& p_c1, const Point p_p1, const Collider& p_c2, const Point& p_p2)
{
	// First check for simple overlap
	if (p_p1.x < p_p2.x + p_c2.m_size.x &&
		p_p1.x + p_c1.m_size.x > p_p2.x &&
		p_p1.y < p_p2.y + p_c2.m_size.y &&
		p_p1.y + p_c1.m_size.y > p_p2.y)
	{
		if (p_c1.m_simple)
		{
			if (p_c2.m_simple)
			{
				// Both colliders are simple to begin with
				return true;
			}
			else
			{
				// `p_c1` is simple, `p_c2` is complex
				for (size_t y = (p_p2.y < p_p1.y ? p_p1.y - p_p2.y : 0); y < p_c2.m_size.x && p_p2.y + y < p_p1.y + p_c1.m_size.y; y++)
					for (size_t x = (p_p2.x < p_p1.x ? p_p1.x - p_p2.x : 0); x < p_c2.m_size.x && p_p2.x + x < p_p1.x + p_c1.m_size.x; x++)
						if (p_c2(x, y))
							return true;
				return false;
			}
		}
		else
		{
			if (p_c2.m_simple)
			{
				// `p_c1` is complex, `p_c2` is simple
				for (size_t y = (p_p1.y < p_p2.y ? p_p2.y - p_p1.y : 0); y < p_c1.m_size.x && p_p1.y + y < p_p2.y + p_c2.m_size.y; y++)
					for (size_t x = (p_p1.x < p_p2.x ? p_p2.x - p_p1.x : 0); x < p_c1.m_size.x && p_p1.x + x < p_p2.x + p_c2.m_size.x; x++)
						if (p_c1(x, y))
							return true;
				return false;
			}
			else
			{
				// `p_c1` and `p_c2` are complex

				size_t y1 = 0;
				size_t y2 = 0;
				// If complex1 is before complex2, then add to y1 so it starts where complex2 starts, and keep y2 0
				if (p_p1.y < p_p2.y)
					y1 = p_p2.y - p_p1.y;
				// Otherwise, if complex2 is before complex1, then add to y2 so it starts where complex1 starts, and keep y1 0
				else if (p_p2.y < p_p1.y)
					y2 = p_p1.y - p_p2.y;

				// Iterate with both Ys until one of them reaches the end of their colliders
				for (; y1 < p_c1.m_size.y && y2 < p_c2.m_size.y; y1++, y2++)
				{
					size_t x1 = 0;
					size_t x2 = 0;
					// If complex1 is before complex2, then add to x1 so it starts where complex2 starts, and keep x2 0
					if (p_p1.x < p_p2.x)
						x1 = p_p2.x - p_p1.x;
					// Otherwise, if complex2 is before complex1, then add to x2 so it starts where complex1 starts, and keep x1 0
					else if (p_p2.x < p_p1.x)
						x2 = p_p1.x - p_p2.x;

					// Iterate with both Xs until one of them reaches the end of their colliders
					for (; x1 < p_c1.m_size.x && x2 < p_c2.m_size.x; x1++, x2++)
						if (p_c1(x1, y1) && p_c2(x2, y2))
							return true;
				}
				return false;
			}
		}
	}
	else
	{
		// If there's not even simple overlap, there will be no complex overlap as well
		return false;
	}
}

void KTech::Collision::ExpandMovementTree(ID<Object>& p_thisObjID, Point p_dir,
	std::vector<CollisionData>& p_pushData,
	std::vector<CollisionData>& p_blockData,
	std::vector<CollisionData>& p_overlapData,
	std::vector<CollisionData>& p_exitOverlapData)
{
	Object* thisObj = OBJECTS[p_thisObjID];
	Layer* layer = LAYERS[thisObj->m_parentLayer];

	for (size_t o = 0; o < layer->m_objects.size(); o++) // Other objects
	{
		ID<Object>& otherObjID = (*layer)[o];

		// Filter in only objects which aren't a part of the movement tree and objects from `blockingObjects`
		bool alreadyMoving = false;
		for (size_t i = 0; i < p_pushData.size(); i++) {
			if (otherObjID == p_pushData[i].activeObject || otherObjID == p_pushData[i].passiveObject) {
				alreadyMoving = true;
				break;
			}
		}
		if (alreadyMoving || otherObjID == p_thisObjID)
			continue;
		
		Object* otherObj = OBJECTS[otherObjID];

		// *Needed in this scope!
		CR cr = CR::O; // The collision result between this object and this other object. Default - overlap (nothing, ignored).
		size_t c = 0; // Collider index 
		size_t oc = 0; // Other collider index
		size_t originallyBlockedColliderI = 0; // Used if not found a pushable collider
		size_t originallyBlockingColliderI = 0; // ^

		for (size_t c = 0; c < thisObj->m_colliders.size(); c++) // This object's colliders
		{
			// Set collider
	 		const Collider& col = (thisObj->m_colliders[c]);
			// Skip this one if it is invalid
			if (!col.m_active || col.m_size.x == 0 || col.m_size.y == 0)
				continue;

			for (oc = 0; oc < otherObj->m_colliders.size(); oc++) // Other object's colliders
			{
				// Set other collider
				const Collider& otherCol = otherObj->m_colliders[oc];
				// Skip this one if it is invalid
				if (!otherCol.m_active || otherCol.m_size.x == 0 || otherCol.m_size.y == 0)
					continue;

				// Get the potential collision result in advance
				CR potentialCollisionResult = GetPotentialCollisionResult(col.m_type, otherCol.m_type);

				// If the current known result is block (0) and the potential result of this other object is also block then there is no reason to proceed checking.
				// If the current known result if push (1) and the potential result is also push then once again there is no reason to proceed checking.
				// But, if the potential result is overlap then proceed checking no matter the known result, we want to get all the overlap occurrences.
				
				// Check enter/exit overlap events
				if (potentialCollisionResult == CR::O)
				{
					// Compare current and future overlapping according to the types of the colliders
					bool currentOverlapState = AreCollidersOverlapping(col, thisObj->m_pos + col.m_rPos, otherCol, otherObj->m_pos + otherCol.m_rPos);
					bool futureOverlapState = AreCollidersOverlapping(col, thisObj->m_pos + col.m_rPos + p_dir, otherCol, otherObj->m_pos);

					if (!currentOverlapState && futureOverlapState)
						p_overlapData.push_back({p_thisObjID, otherObjID, c, oc}); // Entered overlap
					else if (currentOverlapState && !futureOverlapState)
						p_exitOverlapData.push_back({p_thisObjID, otherObjID, c, oc}); // Exited overlap
				}
				// Proceed to checking collision (repositioned overlap) only if the potential collision result is not already known.
				// This means that if the currently known CR is a block and the potential CR is also a block, then there is no reason to check collision again since encountering another blocking collider doesn't matter.
				// And, if the currently known CR is a push, then there is no reason to check for blocks or more ways to push, since the other object is going to get pushed either way.
				// But, if the currently known CR is a push, the for loops should still keep going in order to check for more overlaps, which are always relevant.
				// Basically, by detecting a block, this part of the code gets locked on a block, until a push is found, which causes this part of the code to get locked on a push and never check for more collisions again in the current `otherObj`'s iteration.
				else if (cr != potentialCollisionResult)
				{
					if (AreCollidersOverlapping(col, thisObj->m_pos + col.m_rPos + p_dir, otherCol, otherObj->m_pos + otherCol.m_rPos))
					{
						// Collision!
						cr = potentialCollisionResult;
						if (cr == CR::B)
						{
							// There can only be one time (for each other object) where a block result can get to this point in the current `otherObj`'s iteration.
							// That means that `c` and `oc` represent the original blocked and blocking colliders, which can be used for user-defined functions calling.
							originallyBlockedColliderI = c;
							originallyBlockingColliderI = oc;
						}
					}
				}
			}
		}

		if (cr == CR::B)
			p_blockData.push_back({p_thisObjID, otherObjID, originallyBlockedColliderI, originallyBlockingColliderI});
		else if (cr == CR::P)
			p_pushData.push_back({p_thisObjID, otherObjID, oc, c});
	}
}