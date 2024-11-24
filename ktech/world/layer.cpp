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

#include "layer.hpp"

#include "../utility/rgbcolors.hpp"
#include "map.hpp"
#include "object.hpp"
#include "../engine/output.hpp"
#include "../engine/engine.hpp"

KTech::Layer::Layer(Engine& p_engine, std::string p_name)
	: engine(p_engine), m_name(std::move(p_name))
{
	engine.memory.layers.Add(this);
}

KTech::Layer::Layer(Engine& p_engine, ID<Map>& p_parentMap, std::string p_name)
	: Layer(p_engine, std::move(p_name))
{
	EnterMap(p_parentMap);
}

KTech::Layer::~Layer()
{
	Output::Log("<Layer[" + m_name + "]::~Layer()>", RGBColors::red);
	RemoveAllObjects();
	LeaveMap();
	engine.memory.layers.Remove(m_id);
}

auto KTech::Layer::operator[](size_t p_index) -> ID<Object>&
{
	return m_objects[p_index];
}

auto KTech::Layer::AddObject(ID<Object>& p_object) -> bool
{
	if (!engine.memory.objects.Exists(p_object))
	{
		return false;
	}
	for (ID<Object>& object : m_objects)
	{
		if (object == p_object)
		{
			return false;
		}
	}
	engine.memory.objects[p_object]->m_parentLayer = m_id;
	m_objects.push_back(p_object);
	return true;
}

auto KTech::Layer::RemoveObject(ID<Object>& p_object) -> bool
{
	for (size_t i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i] == p_object)
		{
			if (engine.memory.objects.Exists(m_objects[i]))
			{
				engine.memory.objects[m_objects[i]]->m_parentLayer = nullID<Layer>;
			}
			m_objects.erase(m_objects.begin() + i);
			return true;
		}
	}
	return false;
}

auto KTech::Layer::RemoveAllObjects() -> bool
{
	if (m_objects.empty())
	{
		return false;
	}
	for (auto & object : m_objects)
	{
		if (engine.memory.objects.Exists(object))
		{
			engine.memory.objects[object]->m_parentLayer = nullID<Layer>;
		}
	}
	m_objects.clear();
	return true;
}

auto KTech::Layer::EnterMap(ID<Map>& p_map) -> bool
{
	if (p_map == m_parentMap || !engine.memory.maps.Exists(p_map))
	{
		return false;
	}
	return engine.memory.maps[p_map]->AddLayer(m_id);
}

auto KTech::Layer::LeaveMap() -> bool
{
	if (engine.memory.maps.Exists(m_parentMap))
	{
		return engine.memory.maps[m_parentMap]->RemoveLayer(m_id);
	}
	m_parentMap = nullID<Map>;
	return true;
}