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

KTech::Object::Object(Engine& p_engine, Point p_pos, const std::string& p_name)
	: engine(p_engine), m_pos(p_pos), m_name(p_name)
{
	engine.memory.objects.Add(this);
}

KTech::Object::Object(Engine& p_engine, ID<Layer>& p_layer, Point p_pos, const std::string& p_name)
	: Object(p_engine, p_pos, p_name)
{
	EnterLayer(p_layer);
}

KTech::Object::~Object()
{
	LeaveLayer();	
	engine.memory.objects.Remove(m_id);
}

bool KTech::Object::EnterLayer(ID<Layer>& p_layer)
{
	if (p_layer == m_parentLayer || !engine.memory.layers.Exists(p_layer))
		return false;
	return engine.memory.layers[p_layer]->AddObject(m_id);
}

bool KTech::Object::LeaveLayer()
{
	if (engine.memory.layers.Exists(m_parentLayer))
		return engine.memory.layers[m_parentLayer]->RemoveObject(m_id);
	m_parentLayer = nullID<Layer>;
	return true;
}

bool KTech::Object::Move(Point p_dir)
{
	// Request the collision manager of the engine to move this object.
	return engine.collision.MoveObject(m_id, p_dir);
}