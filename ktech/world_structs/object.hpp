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
#include "../misc/id.hpp"
#include "../basic_structs/point.hpp"
#include "../world_structs/texture.hpp"
#include "../world_structs/collider.hpp"

struct KTech::Object
{
	Engine& engine;
	ID<Object> id;

	ID<Layer> parentLayer;

	std::string name = "";
	Point pos = Point(0, 0);

	std::vector<Texture> textures = {};
	std::vector<Collider> colliders = {};

	bool Move(Point dir);
	
	void EnterLayer(ID<Layer>& layer);

	virtual void OnTick() {} // Called by `Map::CallOnTicks()`.
	virtual void OnPushed(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) pushed this object.
	virtual void OnPush(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object pushed a different object (`otherObject`)
	virtual void OnBlocked(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) blocked this object
	virtual void OnBlock(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object blocked a different object (`otherObject`)
	virtual void OnOverlap(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object entered an overlap with a different object (`otherObject`)
	virtual void OnOverlapExit(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // This object exited an overlap with a different object (`otherObject`)
	virtual void OnOverlapped(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) entered an overlap with this object
	virtual void OnOverlappedExit(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider) {} // A different object (`otherObject`) exited an overlap with this object
	
	Object(Engine& engine, Point position = Point(0, 0), const std::string& name = "");
	Object(Engine& engine, ID<Layer>& parentLayer, Point position = Point(0, 0), const std::string& name = "");
	~Object();
};