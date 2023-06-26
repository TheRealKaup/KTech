#include "engine.hpp"

using namespace Engine;

void Engine::Object::ExpandMovementTree(Vector2D dir,
	std::vector<Object*>* pushingObjects,
	std::vector<Object*>* objectsToPush,
	std::vector<size_t>* pushingColliders,
	std::vector<size_t>* collidersToPush,
	std::vector<Object*>* blockedObjects,
	std::vector<Object*>* blockingObjects,
	std::vector<size_t>* blockedColliders,
	std::vector<size_t>* blockingColliders,
	std::vector<Object*>* overlappingObjects,
	std::vector<Object*>* overlappedObjects,
	std::vector<size_t>* overlappingColliders,
	std::vector<size_t>* overlappedColliders,
	std::vector<Object*>* exitOverlappingObjects,
	std::vector<Object*>* exitOverlappedObjects,
	std::vector<size_t>* exitOverlappingColliders,
	std::vector<size_t>* exitOverlappedColliders)
{
	// Only supporting SIMPLE COLLLIDERS.

	Object* oObj;
	Collider* collider;
	Collider* otherCollider;

	for (size_t o = 0; o < parentLayer->objects.size(); o++)
	{
		oObj = parentLayer->objects[o];

		// Filter in only objects which aren't a part of the movement tree and objects from `blockingObjects`
		for (int i = 0; i < pushingObjects->size(); i++) {
			if (oObj == (*pushingObjects)[i] || oObj == (*objectsToPush)[i]) {
				oObj = this; // Stupid way to exit the other for-loop but whatever.
			}
		}
		if (oObj == this)
			continue;

		unsigned char finalResult = 2; // Default - nothing
		unsigned c = 0;
		unsigned oc = 0;
		unsigned originallyBlockedColliderI = 0; // Used if not found a pushable collider
		unsigned originallyBlockingColliderI = 0; // ^

		for (c = 0; c < colliders.size(); c++)
		{
			collider = &colliders[c];

			if (!collider->active || collider->size.x == 0 || collider->size.y == 0)
				continue;

			for (oc = 0; oc < oObj->colliders.size(); oc++)
			{
				otherCollider = &oObj->colliders[oc];

				if (!otherCollider->active || otherCollider->size.x == 0 || otherCollider->size.y == 0)
					continue;

				unsigned char collisionResult = 2;
				if (collider->type >= 0 && collider->type < colliderTypes.size())
					if (otherCollider->type >= 0 && otherCollider->type < colliderTypes[collider->type].size())
						collisionResult = colliderTypes[collider->type][otherCollider->type];

				// If the current known result is block (0) and the potential result of this other object is also block then there is no reason to proceed checking.
				// If the current known result if push (1) and the potential result is also push then once again there is no reason to proceed checking.
				// But, if the potential result is overlap then proceed checking no matter the known result, we want to get all the overlaps occurences.
				
				if (collisionResult == 2)
				{
					long cldrPosX = collider->pos.x + pos.x;
					long oCldrPosX = otherCollider->pos.x + oObj->pos.x;
					long cldrPosY = collider->pos.y + pos.y;
					long oCldrPosY = otherCollider->pos.y + oObj->pos.y;

					bool inOverlap = (cldrPosX < oCldrPosX + (int)otherCollider->size.x && cldrPosX + (int)collider->size.x > oCldrPosX
						&& cldrPosY < oCldrPosY + (int)otherCollider->size.y && cldrPosY + (int)collider->size.y > oCldrPosY);

					cldrPosX += dir.x;
					cldrPosY += dir.y;

					if (cldrPosX < oCldrPosX + (int)otherCollider->size.x && cldrPosX + (int)collider->size.x > oCldrPosX
						&& cldrPosY < oCldrPosY + (int)otherCollider->size.y && cldrPosY + (int)collider->size.y > oCldrPosY)
					{
						// Overlap!
						if (!inOverlap)
						{
							overlappingObjects->push_back(this);
							overlappedObjects->push_back(oObj);
							overlappingColliders->push_back(c);
							overlappedColliders->push_back(oc);
						}
					}
					else if (inOverlap)
					{
						exitOverlappingObjects->push_back(this);
						exitOverlappedObjects->push_back(oObj);
						exitOverlappingColliders->push_back(c);
						exitOverlappedColliders->push_back(oc);
					}
				}
				else if (finalResult != collisionResult)
				{
					long cldrPosX = collider->pos.x + pos.x + dir.x;
					long oCldrPosX = otherCollider->pos.x + oObj->pos.x;
					long cldrPosY = collider->pos.y + pos.y + dir.y;
					long oCldrPosY = otherCollider->pos.y + oObj->pos.y;
					if (cldrPosX < oCldrPosX + (int)otherCollider->size.x && cldrPosX + (int)collider->size.x > oCldrPosX
						&& cldrPosY < oCldrPosY + (int)otherCollider->size.y && cldrPosY + (int)collider->size.y > oCldrPosY)
					{
						// Collision!
						finalResult = collisionResult;
						if (finalResult == 0)
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

		if (finalResult == 0)
		{
			blockedObjects->push_back(this);
			blockingObjects->push_back(oObj);
			blockedColliders->push_back(originallyBlockedColliderI);
			blockingColliders->push_back(originallyBlockingColliderI);
		}
		else if (finalResult == 1)
		{
			pushingObjects->push_back(this);
			objectsToPush->push_back(oObj);
			collidersToPush->push_back(c);
			pushingColliders->push_back(oc);
		}
	}
}

bool Object::Move(Vector2D dir)
{
	// Pushing vectors:
	std::vector<Object*> pushingObjects;
	std::vector<Object*> objectsToPush;
	std::vector<size_t> pushingColliders;
	std::vector<size_t> collidersToPush;
	// Blocking vectors
	std::vector<Object*> blockedObjects;
	std::vector<Object*> blockingObjects;
	std::vector<size_t> blockedColliders;
	std::vector<size_t> blockingColliders;
	// Overlapping vectors:
	std::vector<Object*> overlappingObjects;
	std::vector<Object*> overlappedObjects;
	std::vector<size_t> overlappingColliders;
	std::vector<size_t> overlappedColliders;
	std::vector<Object*> exitOverlappingObjects;
	std::vector<Object*> exitOverlappedObjects;
	std::vector<size_t> exitOverlappingColliders;
	std::vector<size_t> exitOverlappedColliders;
	
	ExpandMovementTree(dir, &pushingObjects, &objectsToPush, &pushingColliders, &collidersToPush, &blockedObjects, &blockingObjects, &blockedColliders, &blockingColliders, &overlappingObjects, &overlappedObjects, &overlappingColliders, &overlappedColliders, &exitOverlappingObjects, &exitOverlappedObjects, &exitOverlappingColliders, &exitOverlappedColliders);

	for (size_t i = 0; i < objectsToPush.size(); i++)
		objectsToPush[i]->ExpandMovementTree(dir, &pushingObjects, &objectsToPush, &pushingColliders, &collidersToPush, &blockedObjects, &blockingObjects, &blockedColliders, &blockingColliders, &overlappingObjects, &overlappedObjects, &overlappingColliders, &overlappedColliders, &exitOverlappingObjects, &exitOverlappedObjects, &exitOverlappingColliders, &exitOverlappedColliders);

	if (blockedObjects.size() == 0)
	{
		// Able to move.
		// Change positions
		for (size_t i = 0; i < objectsToPush.size(); i++)
		{
			objectsToPush[i]->pos.x += dir.x;
			objectsToPush[i]->pos.y += dir.y;
		}
		pos.x += dir.x;
		pos.y += dir.y;
		// Call push events
		for (size_t i = 0; i < objectsToPush.size(); i++)
		{
			pushingObjects[i]->lastPush = dir;
			pushingObjects[i]->colliderIndex = pushingColliders[i];
			pushingObjects[i]->otherObject = objectsToPush[i];
			pushingObjects[i]->otherColliderIndex = collidersToPush[i];

			objectsToPush[i]->lastPush = dir;
			objectsToPush[i]->colliderIndex = collidersToPush[i];
			objectsToPush[i]->otherObject = pushingObjects[i];
			objectsToPush[i]->otherColliderIndex = pushingColliders[i];
			
			if (pushingObjects[i]->OnPush)
				pushingObjects[i]->OnPush();

			if (objectsToPush[i]->OnPushed)
				objectsToPush[i]->OnPushed();
		}
		// Call overlap events
		for (size_t i = 0; i < overlappingObjects.size(); i++)
		{
			overlappingObjects[i]->lastPush = dir;
			overlappingObjects[i]->colliderIndex = overlappingColliders[i];
			overlappingObjects[i]->otherObject = overlappedObjects[i];
			overlappingObjects[i]->otherColliderIndex = overlappedColliders[i];

			overlappedObjects[i]->lastPush = dir;
			overlappedObjects[i]->colliderIndex = overlappedColliders[i];
			overlappedObjects[i]->otherObject = overlappingObjects[i];
			overlappedObjects[i]->otherColliderIndex = overlappingColliders[i];

			if (overlappingObjects[i]->OnOverlap)
				overlappingObjects[i]->OnOverlap();

			if (overlappedObjects[i]->OnOverlapped)
				overlappedObjects[i]->OnOverlapped();
		}
		// Call overlap exit events
		for (size_t i = 0; i < exitOverlappingObjects.size(); i++)
		{
			exitOverlappingObjects[i]->lastPush = dir;
			exitOverlappingObjects[i]->colliderIndex = exitOverlappingColliders[i];
			exitOverlappingObjects[i]->otherObject = exitOverlappedObjects[i];
			exitOverlappingObjects[i]->otherColliderIndex = exitOverlappedColliders[i];

			exitOverlappedObjects[i]->lastPush = dir;
			exitOverlappedObjects[i]->colliderIndex = exitOverlappedColliders[i];
			exitOverlappedObjects[i]->otherObject = exitOverlappingObjects[i];
			exitOverlappedObjects[i]->otherColliderIndex = exitOverlappingColliders[i];

			if (exitOverlappingObjects[i]->OnOverlapExit)
				exitOverlappingObjects[i]->OnOverlapExit();

			if (exitOverlappedObjects[i]->OnOverlappedExit)
				exitOverlappedObjects[i]->OnOverlappedExit();
		}
		return true;
	}
	else
	{
		// Unable to move, blocked.
		Object* theOtherObject = NULL;
		Vector2D lastPush = { 0, 0 };
		int theColliderIndex = -1;
		int theOtherColliderIndex = -1;
		for (size_t i = 0; i < blockedObjects.size(); i++)
		{
			blockedObjects[i]->lastPush = dir;
			blockedObjects[i]->colliderIndex = blockedColliders[i];
			blockedObjects[i]->otherObject = blockingObjects[i];
			blockedObjects[i]->otherColliderIndex = blockingColliders[i];

			blockingObjects[i]->lastPush = dir;
			blockingObjects[i]->colliderIndex = blockingColliders[i];
			blockingObjects[i]->otherObject = blockedObjects[i];
			blockingObjects[i]->otherColliderIndex = blockedColliders[i];

			if (blockedObjects[i]->OnBlocked)
				blockedObjects[i]->OnBlocked();
			if (blockingObjects[i]->OnBlock)
				blockingObjects[i]->OnBlock();
		}
		return false; 
	}
}