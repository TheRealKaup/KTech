#include "engine.hpp"

using namespace Engine;

struct CollisionData{
	Object* activeObject;
	Object* passiveObject;
	size_t activeCollider;
	size_t passiveCollider;
};

void ExpandMovementTree(Object* thisObj, Point dir,
	std::vector<CollisionData>& pushData,
	std::vector<CollisionData>& blockData,
	std::vector<CollisionData>& overlapData,
	std::vector<CollisionData>& exitOverlapData)
{
	// Only supporting SIMPLE COLLLIDERS.
	Object* oObj;
	Collider* collider;
	Collider* otherCollider;

	for (size_t o = 0; o < thisObj->parentLayer->objects.size(); o++)
	{
		oObj = thisObj->parentLayer->objects[o];

		// Filter in only objects which aren't a part of the movement tree and objects from `blockingObjects`
		for (size_t i = 0; i < pushData.size(); i++) {
			if (oObj == pushData[i].activeObject || oObj == pushData[i].passiveObject) {
				oObj = thisObj; // Stupid way to exit the other for-loop but whatever.
			}
		}
		if (oObj == thisObj)
			continue;

		CR finalResult = CR::O; // Default - nothing
		size_t c = 0;
		size_t oc = 0;
		size_t originallyBlockedColliderI = 0; // Used if not found a pushable collider
		size_t originallyBlockingColliderI = 0; // ^

		for (c = 0; c < thisObj->colliders.size(); c++)
		{
			collider = &thisObj->colliders[c];

			if (!collider->active || collider->size.x == 0 || collider->size.y == 0)
				continue;

			for (oc = 0; oc < oObj->colliders.size(); oc++)
			{
				otherCollider = &oObj->colliders[oc];

				if (!otherCollider->active || otherCollider->size.x == 0 || otherCollider->size.y == 0)
					continue;

				CR collisionResult = CR::O;
				if (collider->type >= 0 && collider->type < colliderTypes.size())
					if (otherCollider->type >= 0 && otherCollider->type < colliderTypes[collider->type].size())
						collisionResult = colliderTypes[collider->type][otherCollider->type];

				// If the current known result is block (0) and the potential result of this other object is also block then there is no reason to proceed checking.
				// If the current known result if push (1) and the potential result is also push then once again there is no reason to proceed checking.
				// But, if the potential result is overlap then proceed checking no matter the known result, we want to get all the overlaps occurences.
				
				if (collisionResult == CR::O)
				{
					int32_t cldrPosX = collider->pos_r.x + thisObj->pos.x;
					int32_t oCldrPosX = otherCollider->pos_r.x + oObj->pos.x;
					int32_t cldrPosY = collider->pos_r.y + thisObj->pos.y;
					int32_t oCldrPosY = otherCollider->pos_r.y + oObj->pos.y;

					bool inOverlap = (cldrPosX < oCldrPosX + otherCollider->size.x && cldrPosX + collider->size.x > oCldrPosX
						&& cldrPosY < oCldrPosY + otherCollider->size.y && cldrPosY + collider->size.y > oCldrPosY);

					cldrPosX += dir.x;
					cldrPosY += dir.y;

					if (cldrPosX < oCldrPosX + otherCollider->size.x && cldrPosX + collider->size.x > oCldrPosX
						&& cldrPosY < oCldrPosY + otherCollider->size.y && cldrPosY + collider->size.y > oCldrPosY)
					{
						// Overlap!
						if (!inOverlap)
						{
							overlapData.push_back({thisObj, oObj, c, oc});
						}
					}
					else if (inOverlap)
					{
						exitOverlapData.push_back({thisObj, oObj, c, oc});
					}
				}
				else if (finalResult != collisionResult)
				{
					int32_t cldrPosX = collider->pos_r.x + thisObj->pos.x + dir.x;
					int32_t oCldrPosX = otherCollider->pos_r.x + oObj->pos.x;
					int32_t cldrPosY = collider->pos_r.y + thisObj->pos.y + dir.y;
					int32_t oCldrPosY = otherCollider->pos_r.y + oObj->pos.y;
					if (cldrPosX < oCldrPosX + otherCollider->size.x && cldrPosX + collider->size.x > oCldrPosX
						&& cldrPosY < oCldrPosY + otherCollider->size.y && cldrPosY + collider->size.y > oCldrPosY)
					{
						// Collision!
						finalResult = collisionResult;
						if (finalResult == CR::B)
						{
							// There can only be one time (for each other object) where a block situation can get to this point in the code.
							// That means that `c` and `oc` represent the original blocked and blocking colliders, which can be used for user-defined functions calling.
							originallyBlockedColliderI = c;
							originallyBlockingColliderI = oc;
						}
					}
				}
			}
		}

		if (finalResult == CR::B)
		{
			blockData.push_back({thisObj, oObj, originallyBlockedColliderI, originallyBlockingColliderI});
		}
		else if (finalResult == CR::P)
		{
			pushData.push_back({thisObj, oObj, oc, c});
		}
	}
}

bool Object::Move(Point dir)
{
	std::vector<CollisionData> pushData;
	std::vector<CollisionData> blockData;
	std::vector<CollisionData> overlapData;
	std::vector<CollisionData> exitOverlapData;
	
	ExpandMovementTree(this, dir, pushData, blockData, overlapData, exitOverlapData);

	for (size_t i = 0; i < pushData.size(); i++)
		ExpandMovementTree(pushData[i].passiveObject, dir, pushData, blockData, overlapData, exitOverlapData);

	if (blockData.size() == 0)
	{
		// Able to move.
		// Change positions
		for (size_t i = 0; i < pushData.size(); i++)
		{
			pushData[i].passiveObject->pos.x += dir.x;
			pushData[i].passiveObject->pos.y += dir.y;
		}
		pos.x += dir.x;
		pos.y += dir.y;
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
			
			if (pushData[i].activeObject->OnPush)
				pushData[i].activeObject->OnPush();

			if (pushData[i].passiveObject->OnPushed)
				pushData[i].passiveObject->OnPushed();
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

			if (overlapData[i].activeObject->OnOverlap)
				overlapData[i].activeObject->OnOverlap();

			if (overlapData[i].passiveObject->OnOverlapped)
				overlapData[i].passiveObject->OnOverlapped();
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

			if (exitOverlapData[i].activeObject->OnOverlapExit)
				exitOverlapData[i].activeObject->OnOverlapExit();

			if (exitOverlapData[i].passiveObject->OnOverlappedExit)
				exitOverlapData[i].passiveObject->OnOverlappedExit();
		}
		return true;
	}
	else
	{
		// Unable to move, blocked.
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

			if (blockData[i].activeObject->OnBlocked)
				blockData[i].activeObject->OnBlocked();

			if (blockData[i].passiveObject->OnBlock)
				blockData[i].passiveObject->OnBlock();
		}
		return false; 
	}
}