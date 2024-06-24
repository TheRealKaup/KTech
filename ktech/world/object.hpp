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
#include "../utility/id.hpp"
#include "../basic/point.hpp"
#include "collider.hpp"
#include "texture.hpp"

/*!
	\brief Collection of `Texture`s and `Collider`s

	Its `Texture`s and `Collider`s behave relative to their parent `Object`'s position.

	\ingroup world
*/
class KTech::Object
{
public:
	Engine& engine;
	ID<Object> m_id;
	std::string m_name;
	ID<Layer> m_parentLayer;

	Point m_pos; //!< Position
	std::vector<Texture> m_textures = {}; //!< `Texture`s. Note: vector stores instances, not `ID`s
	std::vector<Collider> m_colliders = {}; //!< `Collider`s. Note: vector stores instances, not `ID`s

	Object(Engine& engine, Point position = Point(0, 0), const std::string& name = "");
	Object(Engine& engine, ID<Layer>& parentLayer, Point position = Point(0, 0), const std::string& name = "");
	virtual ~Object();

	bool EnterLayer(ID<Layer>& layer);
	bool LeaveLayer();

	//! Calls Collision::MoveObject() on itself.
	bool Move(Point direction);
	
protected:
	inline virtual bool OnTick() { return false; };
	/*!@{
		\name Collision callback functions
		These functions may be called by Collision::MoveObject() after processing the movement of this `Object` or another.
		\param[in] direction The moved direction (already applied to this and the other `Object`s)
		\param[in] collider The index of this `Object`'s `Collider` which actually collided
		\param[in] otherObject The `Object` that collided with this `Object`
		\param[in] otherCollider The index of the other `Object`'s `Collider` which actually collided
	*/
	//!	Got pushed by `otherObject`
	inline virtual void OnPushed(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) pushed this object.
	//! Pushed `otherObject`
	inline virtual void OnPush(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object pushed a different object (`otherObject`)
	//! Got blocked by `otherObject`
	inline virtual void OnBlocked(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) blocked this object
	//! Blocked `otherObject`
	inline virtual void OnBlock(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object blocked a different object (`otherObject`)
	//! Overlapped `otherObject`
	inline virtual void OnOverlap(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object entered an overlap with a different object (`otherObject`)
	//! Exited from overlap with `otherObject`
	inline virtual void OnOverlapExit(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object exited an overlap with a different object (`otherObject`)
	//! Got overlapped by `otherObject` 
	inline virtual void OnOverlapped(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) entered an overlap with this object
	//! Got exited from overlap with `otherObject`
	inline virtual void OnOverlappedExit(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) exited an overlap with this object
	/*!@}*/
	friend class KTech::Collision;
	friend class KTech::Memory;
};