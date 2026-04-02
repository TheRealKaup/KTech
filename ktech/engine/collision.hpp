/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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
	/*!
		@var `Collision::CR`

		@brief Collision result.

		There are 3 possible collision results: `CR::B` (block), `CR::P` (push), and `CR::O` (overlap).

		@see `Collision::colliderTypes`
	*/
	enum class CR : uint8_t
	{
		B, //!< Block; the moving collider can't move because the passive collider can't be moved.
		P, //!< Push; the moving collider can make way by moving the passive collider with it.
		O  //!< Overlap; the moving collider can move into the passive collider.
	};

	/*!
		@var `Collision::colliderTypes`

		@brief Matrix representing collider types and their potential collision results.

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
	std::vector<std::vector<CR>> colliderTypes{
		{CR::B, CR::P, CR::O}, // Unpushable - 0
		{CR::B, CR::P, CR::O}, // Pushable - 1
		{CR::O, CR::O, CR::O}  // Overlapping - 2
	};

	/*!
		@brief Move an `Object` in relation to other `Object`s from the same `Layer`.

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
	auto MoveObject(const ID<Object>& object, Point direction) -> bool;

private:
	Engine& engine;

	struct CollisionData
	{
		ID<Object> activeObject;
		ID<Object> passiveObject;
		size_t activeCollider;
		size_t passiveCollider;
	};

	Collision(Engine& engine)
		: engine(engine) {};

	auto GetPotentialCollisionResult(uint8_t type1, uint8_t type2) -> CR;
	// Warning: `position1` and `position2` override `collider1.m_rPos` and `collider2.m_rPos` respectively
	static auto AreCollidersOverlapping(
		const Collider& collider1, const Point& position1, const Collider& collider2, const Point& position2
	) -> bool;
	static auto AreSimpleCollidersOverlapping(
		const Collider& collider1, const Point& position1, const Collider& collider2, const Point& position2
	) -> bool;
	static auto AreSimpleAndComplexCollidersOverlapping(
		const Collider& complex, const Point& complexPosition, const Collider& simple, const Point& simplePosition
	) -> bool;
	static auto AreComplexCollidersOverlapping(
		const Collider& collider1, const Point& position1, const Collider& collider2, const Point& position2
	) -> bool;
	void ExpandMovementTree(
		const ID<Object>& thisObject,
		Point direction,
		std::vector<CollisionData>& pushData,
		std::vector<CollisionData>& blockData,
		std::vector<CollisionData>& overlapData,
		std::vector<CollisionData>& exitOverlapData
	);

	friend class Engine;
};
