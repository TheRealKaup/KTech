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
	@brief Define KTech::Layer members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "layer.hpp"

#include "../engine/engine.hpp"
#include "../engine/output.hpp"
#include "../utility/rgbcolors.hpp"
#include "map.hpp"
#include "object.hpp"

KTech::Layer::Layer(Engine& p_engine, std::string p_name)
	: Entity(p_engine, std::move(p_name))
{}

KTech::Layer::Layer(Engine& p_engine, const ID<Map>& p_parentMap, std::string p_name)
	: Layer(p_engine, std::move(p_name))
{
	EnterMap(p_parentMap);
}

KTech::Layer::~Layer()
{
	Output::Log("<Layer[" + m_name + "]::~Layer()>", RGBColors::red);
	RemoveAllObjects();
	LeaveMap();
}

auto KTech::Layer::operator[](size_t p_index) -> ID<Object>&
{
	return m_objects.m_subs[p_index];
}

auto KTech::Layer::AddObject(const ID<Object>& p_object) -> bool
{
	return m_objects.Add(m_engine, m_id, p_object, &Object::m_parentLayer, &Object::LeaveLayer);
}

auto KTech::Layer::RemoveObject(const ID<Object>& p_object) -> bool
{
	return m_objects.Remove(m_engine, p_object, &Object::m_parentLayer);
}

auto KTech::Layer::RemoveAllObjects() -> bool
{
	return m_objects.RemoveAll(m_engine, &Object::m_parentLayer);
}

auto KTech::Layer::EnterMap(const ID<Map>& p_map) -> bool
{
	return m_engine.memory[p_map]->AddLayer(m_id);
}

auto KTech::Layer::LeaveMap() -> bool
{
	if (m_parentMap == nullID<Map>)
	{
		return false;
	}
	return m_engine.memory[m_parentMap]->RemoveLayer(m_id);
}
