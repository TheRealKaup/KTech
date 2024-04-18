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

KTech::Layer::Layer(Engine& p_engine, const std::string& p_name)
	: engine(p_engine), m_name(p_name)
{
	engine.memory.layers.Add(this);
}

// Currently layers must be forced to be added to maps ASAP, otherwise there would be seg faults, for example, whenever objects try to move (since they can't access engine.collision). 
KTech::Layer::Layer(Engine& p_engine, ID<Map>& p_map, const std::string& p_name)
	: engine(p_engine), m_name(p_name)
{
	engine.memory.layers.Add(this);
	EnterMap(p_map);
}

KTech::Layer::~Layer()
{
	// Reset the `parentLayer` of the objects that have it set to this layer
	Output::Log("<Layer[" + m_name + "]::~Layer()>", RGBColors::red);
	for (ID<Object>& obj : m_objects)
		if (engine.memory.objects.Exists(obj))
			engine.memory.objects[obj]->m_parentLayer = ID<Layer>(0, 0);
	if (engine.memory.maps.Exists(m_parentMap))
		engine.memory.maps[m_parentMap]->RemoveLayer(m_id);
}

KTech::ID<KTech::Object> KTech::Layer::operator[](size_t p_i)
{
	if (p_i < m_objects.size())
		return m_objects[p_i];
	else
		return ID<Object>(0, 0);
}

void KTech::Layer::AddObject(ID<Object>& p_object)
{
	engine.memory.objects[p_object]->m_parentLayer = m_id;
	m_objects.push_back(p_object);
}

bool KTech::Layer::RemoveObject(const std::string& p_name)
{
	for (size_t i = 0; i < m_objects.size(); i++)
	{
		if (engine.memory.objects[m_objects[i]]->m_name == p_name)
		{
			if (engine.memory.objects.Exists(m_objects[i]))
				engine.memory.objects[m_objects[i]]->m_parentLayer = ID<Layer>(0, 0);
			m_objects.erase(m_objects.begin() + i);
			return true;
		}
	}
	return false;
}

bool KTech::Layer::RemoveObject(ID<Object>& p_object)
{
	for (size_t i = 0; i < m_objects.size(); i++)
	{
		if (m_objects[i] == p_object)
		{
			if (engine.memory.objects.Exists(m_objects[i]))
				engine.memory.objects[m_objects[i]]->m_parentLayer = ID<Layer>(0, 0);
			m_objects.erase(m_objects.begin() + i);
			return true;
		}
	}
	return false;
}

void KTech::Layer::EnterMap(ID<Map>& p_map)
{
	if (engine.memory.maps.Exists(m_parentMap))
		engine.memory.maps[m_parentMap]->RemoveLayer(m_id);
	engine.memory.maps[p_map]->AddLayer(m_id);
}