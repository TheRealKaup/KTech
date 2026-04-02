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
#include "../basic/point.hpp"
#include "../utility/id.hpp"
#include "collider.hpp"
#include "texture.hpp"

/*!
	@brief World structure that comprises `Texture`s and `Collider`s, and exists within `Layer`.

	This and `Widget` are the most commonly inherited-from world structures. It differs from `Widget` because it can contain `Collider`s, while `Widget` is limited to `Texture`s. `Widget` also has some additional features which are useful for user-interfaces. You can conveniently make player-controlled classes based on `Object`, such as walking characters (see "simpleplatform" game example).
*/
class KTech::Object
{
public:
	Engine& engine;								 //!< Parent `Engine`.
	const ID<Object> m_id{ID<Object>::Unique()}; //!< Personal `ID`.
	std::string m_name;							 //!< String name.
	ID<Layer> m_parentLayer;					 //!< Parent `Layer`.

	Point m_pos;					   //!< World position.
	std::vector<Texture> m_textures;   //!< `Texture`s.
	std::vector<Collider> m_colliders; //!< `Collider`s.

	/*!
		@fn Object::Object(Engine& engine, Point position, std::string name)
		@brief Construct an `Object`.
		@param engine Parent `Engine`.
		@param position World position.
		@param name String name.
	*/
	Object(Engine& engine, Point position = Point(0, 0), std::string name = "");

	/*!
		@fn Object::Object(Engine& engine, const ID<Layer>& parentLayer, Point position, std::string name)
		@brief Construct an `Object` and immediately enter a `Layer`.
		@param engine Parent `Engine`.
		@param parentLayer `Layer` to enter.
		@param position World position.
		@param name String name.
	*/
	Object(Engine& engine, const ID<Layer>& parentLayer, Point position = Point(0, 0), std::string name = "");

	/*!
		@brief  Leave parent `Layer` (if in one) and remove itself from `Memory`.
	*/
	virtual ~Object();

	/*!
		@fn Object::EnterLayer
		@brief Enter a `Layer`.
		@param layer The `Layer` to enter.
		@return `true` if entered `Layer`. `false` if the given `Layer` is already the parent `Layer`, doesn't exist in `Memory`, or failed to add this `Object`.
	*/
	auto EnterLayer(const ID<Layer>& layer) -> bool;

	/*!
		@brief Leave the parent `Layer`.
		@return `true` if left the parent `Layer`. `false` if the parent `Layer` failed to remove this `Object`.
	*/
	auto LeaveLayer() -> bool;

	/*!
		@brief Move in relation to other `Object`s in the `Layer` and their `Collider`s.

		@details This function calls `Collision::MoveObject()` on itself (on the calling `Object`; `this`).

		@param [in] direction Amount of cells to move the object. For example, Point(2, 3) attempts to move 2 cells right and 3 cells down

		@return `true` if moved. `false` if blocked.

		@see `Collision::MoveObject()`
	*/
	auto Move(Point direction) -> bool;

protected:
	/*!
		@brief Virtual function called once each tick.

		Called by `Memory::CallOnTicks()`.

		@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

		@see `Memory::CallOnTicks()`
		@see `Output::ShouldRenderThisTick()`
	*/
	virtual auto OnTick() -> bool;

	/*!
		@fn KTech::Object::OnMove
		@brief Called by `Collision::MoveObject()` as a result of this `Object` moving (voluntarily or passively).
		@param direction The movement's direction.
	*/
	virtual void OnMove(Point direction);

	/*!
		@fn KTech::Object::OnPushed
		@brief Called by `Collision::MoveObject()` as a result of this `Object` getting pushed.
		@param direction The movement's direction.
		@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
		@param otherObject The `Object` that collided with this `Object`.
		@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
	*/
	virtual void OnPushed(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	/*!
		@fn KTech::Object::OnPush
		@brief Called by `Collision::MoveObject()` as a result of this `Object` pushing another `Object`.
		@param direction The movement's direction.
		@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
		@param otherObject The `Object` that collided with this `Object`.
		@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
	*/
	virtual void OnPush(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	/*!
		@fn KTech::Object::OnBlocked
		@brief Called by `Collision::MoveObject()` as a result of this `Object` getting blocked by another `Object`.
		@param direction The attempted movement's direction.
		@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
		@param otherObject The `Object` that collided with this `Object`.
		@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
	*/
	virtual void OnBlocked(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	/*!
		@fn KTech::Object::OnBlock
		@brief Called by `Collision::MoveObject()` as a result of this `Object` blocking another `Object`.
		@param direction The attempted movement's direction.
		@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
		@param otherObject The `Object` that collided with this `Object`.
		@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
	*/
	virtual void OnBlock(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	/*!
		@fn KTech::Object::OnOverlap
		@brief Called by `Collision::MoveObject()` as a result of this `Object` overlapping into another `Object`.
		@param direction The movement's direction.
		@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
		@param otherObject The `Object` that collided with this `Object`.
		@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
	*/
	virtual void OnOverlap(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	/*!
		@fn KTech::Object::OnOverlapExit
		@brief Called by `Collision::MoveObject()` as a result of this `Object` leaving an overlap with another `Object`.
		@param direction The movement's direction.
		@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
		@param otherObject The `Object` that collided with this `Object`.
		@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
	*/
	virtual void OnOverlapExit(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	/*!
		@fn KTech::Object::OnOverlapped
		@brief Called by `Collision::MoveObject()` as a result of this `Object` getting overlapped into by another `Object`.
		@param direction The movement's direction.
		@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
		@param otherObject The `Object` that collided with this `Object`.
		@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
	*/
	virtual void OnOverlapped(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	/*!
		@fn KTech::Object::OnOverlappedExit
		@brief Called by `Collision::MoveObject()` as a result of **another** `Object` leaving an overlap with **this** `Object`.
		@param direction The movement's direction.
		@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
		@param otherObject The `Object` that collided with this `Object`.
		@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
	*/
	virtual void OnOverlappedExit(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	friend class KTech::Collision;
	friend class KTech::Memory;
};
