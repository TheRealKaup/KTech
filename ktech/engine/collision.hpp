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
#include "../misc/cr.hpp"

#include <vector>

// Exists to store `colliderTypes`.
class KTech::Collision
{
public:
	Engine* engine;

	std::vector<std::vector<CR>> colliderTypes = {
		{ CR::B, CR::P, CR::O }, // Unpushable - 0
		{ CR::B, CR::P, CR::O }, // Pushable - 1
		{ CR::O, CR::O, CR::O } // overlapping - 2
	};

	struct CollisionData{
		ID<Object>& activeObject;
		ID<Object>& passiveObject;
		size_t activeCollider;
		size_t passiveCollider;
	};

	CR GetPotentialCollisionResult(uint8_t t1, uint8_t t2);

	bool MoveObject(ID<Object>& object, Point dir);

	inline Collision(Engine* engine) : engine(engine) {};

private:
	static bool AreSimpleCollidersOverlapping(UPoint simple1, Point pos1, UPoint simple2, Point pos2);
	static bool AreSimpleAndComplexCollidersOverlapping(UPoint simple, Point simplePos, const std::vector<std::vector<bool>>& complex, Point complexPos);
	static bool AreComplexCollidersOverlapping(const std::vector<std::vector<bool>>& c1, Point p1, const std::vector<std::vector<bool>>& c2, Point p2);

	void ExpandMovementTree(ID<Object>& thisObj, Point dir,
		std::vector<CollisionData>& pushData,
		std::vector<CollisionData>& blockData,
		std::vector<CollisionData>& overlapData,
		std::vector<CollisionData>& exitOverlapData);
};