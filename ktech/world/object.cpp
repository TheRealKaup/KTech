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

/*!
	@file
	@brief Define KTech::Object members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "object.hpp"

#include "../engine/engine.hpp"
#include "entity.hpp"
#include "layer.hpp"

KTech::Object::Object(Engine& p_engine, Point p_position, std::string p_name)
	: KTech::ChildEntity<Object, Layer>(p_engine, std::move(p_name)), m_pos(p_position)
{}

KTech::Object::Object(Engine& p_engine, const ID<Layer>& p_parentLayer, Point p_position, std::string p_name)
	: Object(p_engine, p_position, std::move(p_name))
{
	EnterLayer(p_parentLayer);
}

auto KTech::Object::EnterLayer(const ID<Layer>& p_layer) -> bool
{
	return Enter(p_layer);
}

auto KTech::Object::LeaveLayer() -> bool
{
	return Leave();
}

auto KTech::Object::Move(Point p_direction) -> bool
{
	return m_engine.collision.MoveObject(m_id, p_direction);
}

void KTech::Object::OnMove(Point p_direction)
{}

void KTech::Object::OnPushed(Point p_direction, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider)
{}

void KTech::Object::OnPush(Point p_direction, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider)
{}

void KTech::Object::OnBlocked(Point p_direction, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider)
{}

void KTech::Object::OnBlock(Point p_direction, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider)
{}

void KTech::Object::OnOverlap(Point p_direction, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider)
{}

void KTech::Object::OnOverlapExit(
	Point p_direction, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider
)
{}

void KTech::Object::OnOverlapped(Point p_direction, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider)
{}

void KTech::Object::OnOverlappedExit(
	Point p_direction, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider
)
{}
