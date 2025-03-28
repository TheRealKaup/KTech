/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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
#include "../world/object.hpp"

#include <vector>

/*!
	@brief Engine component responsible for processing `Object` movement and collision.
*/
class KTech::Collision
{
public:
	enum class CR : uint8_t
	{
		B,	//!< Block; the moving collider can't move because the passive collider can't be moved.
		P,	//!< Push; the moving collider can make way by moving the passive collider with it.
		O	//!< Overlap; the moving collider can move into the passive collider.
	};

	std::vector<std::vector<CR>> colliderTypes{
		{ CR::B, CR::P, CR::O }, // Unpushable - 0
		{ CR::B, CR::P, CR::O }, // Pushable - 1
		{ CR::O, CR::O, CR::O } // Overlapping - 2
	};

	auto MoveObject(const ID<Object>& object, Point direction) -> bool;

private:
	Engine& engine;

	struct CollisionData{
		ID<Object> activeObject;
		ID<Object> passiveObject;
		size_t activeCollider;
		size_t passiveCollider;
	};

	inline Collision(Engine& engine)
		: engine(engine) {};

	auto GetPotentialCollisionResult(uint8_t type1, uint8_t type2) -> CR;
	// Warning: `position1` and `position2` override `collider1.m_rPos` and `collider2.m_rPos` respectively
	static auto AreCollidersOverlapping(const Collider& collider1, const Point& position1, const Collider& collider2, const Point& position2) -> bool;
	static auto AreSimpleCollidersOverlapping(const Collider& collider1, const Point& position1, const Collider& collider2, const Point& position2) -> bool;
	static auto AreSimpleAndComplexCollidersOverlapping(const Collider& complex, const Point& complexPosition, const Collider& simple, const Point& simplePosition) -> bool;
	static auto AreComplexCollidersOverlapping(const Collider& collider1, const Point& position1, const Collider& collider2, const Point& position2) -> bool;
	void ExpandMovementTree(const ID<Object>& thisObject, Point direction,
		std::vector<CollisionData>& pushData,
		std::vector<CollisionData>& blockData,
		std::vector<CollisionData>& overlapData,
		std::vector<CollisionData>& exitOverlapData);

	friend class Engine;
};