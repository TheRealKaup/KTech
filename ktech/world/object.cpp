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

#include "object.hpp"

#include "layer.hpp"
#include "../engine/engine.hpp"

/*!
	@fn Object::Object(Engine& engine, Point position, std::string name)
	@brief Construct an `Object`.
	@param engine Parent `Engine`.
	@param position World position.
	@param name String name.
*/
KTech::Object::Object(Engine& p_engine, Point p_position, std::string p_name)
	: engine(p_engine), m_pos(p_position), m_name(std::move(p_name))
{
	engine.memory.objects.Add(this);
}

/*!
	@fn Object::Object(Engine& engine, const ID<Layer>& parentLayer, Point position, std::string name)
	@brief Construct an `Object` and immediately enter a `Layer`.
	@param engine Parent `Engine`.
	@param parentLayer `Layer` to enter.
	@param position World position.
	@param name String name.
*/
KTech::Object::Object(Engine& p_engine, const ID<Layer>& p_parentLayer, Point p_position, std::string p_name)
	: Object(p_engine, p_position, std::move(p_name))
{
	EnterLayer(p_parentLayer);
}

//! @brief Leave parent `Layer` (if in one) and remove itself from `Memory`.
KTech::Object::~Object()
{
	LeaveLayer();
	engine.memory.objects.Remove(m_id);
}

/*!
	@fn Object::EnterLayer
	@brief Enter a `Layer`.
	@param layer The `Layer` to enter.
	@return `true` if entered `Layer`. `false` if the given `Layer` is already the parent `Layer`, doesn't exist in `Memory`, or failed to add this `Object`.
*/
auto KTech::Object::EnterLayer(const ID<Layer>& p_layer) -> bool
{
	if (p_layer == m_parentLayer || !engine.memory.layers.Exists(p_layer))
	{
		return false;
	}
	return engine.memory.layers[p_layer]->AddObject(m_id);
}

/*!
	@brief Leave the parent `Layer`.
	@return `true` if left the parent `Layer`. `false` if the parent `Layer` failed to remove this `Object`.
*/
auto KTech::Object::LeaveLayer() -> bool
{
	if (engine.memory.layers.Exists(m_parentLayer))
	{
		return engine.memory.layers[m_parentLayer]->RemoveObject(m_id);
	}
	m_parentLayer = nullID<Layer>;
	return true;
}

/*!
	@fn Object::Move
	@brief Move in relation to other `Object`s in the `Layer` and their `Collider`s.

	This function calls `Collision::MoveObject()` on itself (on the calling `Object`; `this`).

	@return `true` if moved. `false` if blocked.

	@see `Collision::MoveObject()`
*/
auto KTech::Object::Move(Point p_direction) -> bool
{
	// Request the collision manager of the engine to move this object.
	return engine.collision.MoveObject(m_id, p_direction);
}

/*!
	@brief Virtual function called once each tick.

	Called by `Memory::CallOnTicks()`.

	@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

	@see `Memory::CallOnTicks()`
	@see `Output::ShouldRenderThisTick()`
*/
auto KTech::Object::OnTick() -> bool
{
	return false;
};

/*!
	@fn KTech::Object::OnMove
	@brief Called by `Collision::MoveObject()` as a result of this `Object` moving (voluntarily or passively).
	@param direction The movement's direction.
*/
void KTech::Object::OnMove(Point direction) {}

/*!
	@fn KTech::Object::OnPushed
	@brief Called by `Collision::MoveObject()` as a result of this `Object` getting pushed.
	@param direction The movement's direction.
	@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
	@param otherObject The `Object` that collided with this `Object`.
	@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
*/
void KTech::Object::OnPushed(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {}

/*!
	@fn KTech::Object::OnPush
	@brief Called by `Collision::MoveObject()` as a result of this `Object` pushing another `Object`.
	@param direction The movement's direction.
	@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
	@param otherObject The `Object` that collided with this `Object`.
	@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
*/
void KTech::Object::OnPush(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {}

/*!
	@fn KTech::Object::OnBlocked
	@brief Called by `Collision::MoveObject()` as a result of this `Object` getting blocked by another `Object`.
	@param direction The attempted movement's direction.
	@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
	@param otherObject The `Object` that collided with this `Object`.
	@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
*/
void KTech::Object::OnBlocked(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {}

/*!
	@fn KTech::Object::OnBlock
	@brief Called by `Collision::MoveObject()` as a result of this `Object` blocking another `Object`.
	@param direction The attempted movement's direction.
	@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
	@param otherObject The `Object` that collided with this `Object`.
	@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
*/
void KTech::Object::OnBlock(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {}

/*!
	@fn KTech::Object::OnOverlap
	@brief Called by `Collision::MoveObject()` as a result of this `Object` overlapping into another `Object`.
	@param direction The movement's direction.
	@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
	@param otherObject The `Object` that collided with this `Object`.
	@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
*/
void KTech::Object::OnOverlap(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {}

/*!
	@fn KTech::Object::OnOverlapExit
	@brief Called by `Collision::MoveObject()` as a result of this `Object` leaving an overlap with another `Object`.
	@param direction The movement's direction.
	@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
	@param otherObject The `Object` that collided with this `Object`.
	@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
*/
void KTech::Object::OnOverlapExit(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {}

/*!
	@fn KTech::Object::OnOverlapped
	@brief Called by `Collision::MoveObject()` as a result of this `Object` getting overlapped into by another `Object`.
	@param direction The movement's direction.
	@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
	@param otherObject The `Object` that collided with this `Object`.
	@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
*/
void KTech::Object::OnOverlapped(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {}

/*!
	@fn KTech::Object::OnOverlappedExit
	@brief Called by `Collision::MoveObject()` as a result of **another** `Object` leaving an overlap with **this** `Object`.
	@param direction The movement's direction.
	@param collider The index of this `Object`'s collider that collided (`this->Object::m_colliders[collider]`).
	@param otherObject The `Object` that collided with this `Object`.
	@param otherCollider The index of `otherObject`'s collider that collided (`engine.memory.objects[otherObject]->Object::m_colliders[otherCollider]`).
*/
void KTech::Object::OnOverlappedExit(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {}