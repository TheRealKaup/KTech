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

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../utility/cr.hpp"
#include "../world/object.hpp"

#include <vector>

class KTech::Collision
{
public:
	Engine* const engine;

	std::vector<std::vector<CR>> colliderTypes = {
		{ CR::B, CR::P, CR::O }, // Unpushable - 0
		{ CR::B, CR::P, CR::O }, // Pushable - 1
		{ CR::O, CR::O, CR::O } // overlapping - 2
	};

	inline Collision(Engine* engine)
		: engine(engine) {};

	bool MoveObject(ID<Object>& object, Point direction);

private:
	struct CollisionData{
		ID<Object> activeObject;
		ID<Object> passiveObject;
		size_t activeCollider;
		size_t passiveCollider;
	};

	CR GetPotentialCollisionResult(uint8_t t1, uint8_t t2);
	// Warning: `position1` and `position2` override `collider1.m_rPos` and `collider2.m_rPos` respectively
	static bool AreCollidersOverlapping(const Collider& collider1, const Point position1, const Collider& collider2, const Point& position2);

	void ExpandMovementTree(ID<Object>& thisObject,Point direction,
		std::vector<CollisionData>& pushData,
		std::vector<CollisionData>& blockData,
		std::vector<CollisionData>& overlapData,
		std::vector<CollisionData>& exitOverlapData);
};