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
#include "../utility/id.hpp"
#include "../basic/point.hpp"
#include "collider.hpp"
#include "texture.hpp"

/*!
	@brief World structure that comprises `Texture`s and `Collider`s, and exists within `Layer`.

	This and `Widget` are the most commonly inherited-from world structures. It differs from `Widget` because it can contain `Collider`s, while `Widget` is limited to `Texture`s. `Widget` also has some additional features which are useful for user-interfaces. You can conveniently make player-controlled classes based on `Object`, such as walking characters (see "simpleplatform" game example).
*/
class KTech::Object
{
public:
	Engine& engine; //!< Parent `Engine`.
	const ID<Object> m_id{ID<Object>::Unique()}; //!< Personal `ID`.
	std::string m_name; //!< String name.
	ID<Layer> m_parentLayer; //!< Parent `Layer`.

	Point m_pos; //!< World position.
	std::vector<Texture> m_textures = {}; //!< `Texture`s.
	std::vector<Collider> m_colliders = {}; //!< `Collider`s.

	Object(Engine& engine, Point position = Point(0, 0), std::string name = "");
	Object(Engine& engine, const ID<Layer>& parentLayer, Point position = Point(0, 0), std::string name = "");
	virtual ~Object();

	auto EnterLayer(const ID<Layer>& layer) -> bool;
	auto LeaveLayer() -> bool;

	auto Move(Point direction) -> bool;

protected:
	virtual auto OnTick() -> bool;
	virtual void OnMove(Point direction);
	virtual void OnPushed(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);
	virtual void OnPush(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);
	virtual void OnBlocked(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);
	virtual void OnBlock(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);
	virtual void OnOverlap(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);
	virtual void OnOverlapExit(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);
	virtual void OnOverlapped(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);
	virtual void OnOverlappedExit(Point direction, size_t collider, ID<Object> otherObject, size_t otherCollider);

	friend class KTech::Collision;
	friend class KTech::Memory;
};