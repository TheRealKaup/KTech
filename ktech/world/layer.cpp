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

/*!
	@fn Layer::Layer(Engine& engine, std::string name)
	@brief Construct a `Layer`.
	@param [in] engine Parent `Engine`.
	@param [in] name String name.
*/
KTech::Layer::Layer(Engine& p_engine, std::string p_name)
	: engine(p_engine), m_name(std::move(p_name))
{
	engine.memory.layers.Add(this);
}

/*!
	@fn Layer::Layer(Engine& engine, ID<Map>& parentMap, std::string name)
	@brief Construct a `Layer` and immediately enter a `Map`.
	@param [in] engine Parent `Engine`.
	@param [in] parentMap `Map` to enter.
	@param [in] name String name.
*/
KTech::Layer::Layer(Engine& p_engine, const ID<Map>& p_parentMap, std::string p_name)
	: Layer(p_engine, std::move(p_name))
{
	EnterMap(p_parentMap);
}

/*!
	@brief Destruct a `Layer`.

	Removes all contained `Object`s, leaves parent `Map` (if in one), and removes itself from `Memory`.
*/
KTech::Layer::~Layer()
{
	Output::Log("<Layer[" + m_name + "]::~Layer()>", RGBColors::red);
	RemoveAllObjects();
	LeaveMap();
	engine.memory.layers.Remove(m_id);
}

/*!
	@fn Layer::operator[](size_t index)
	@brief Retrieve `Object` `ID` at given index.
	@return Reference to the `ID<Object>` at the given index.
*/
auto KTech::Layer::operator[](size_t p_index) -> ID<Object>&
{
	return m_objects[p_index];
}

/*!
	@fn Layer::AddObject(ID<Object>& object)
	@brief Add an `Object`.
	@param [in] object The `Object` to add.
	@return `true` if added the `Object`. `false` if `Object` doesn't exist in `Memory` or already contained by `Layer`.
*/
auto KTech::Layer::AddObject(const ID<Object>& p_object) -> bool
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

/*!
	@fn Layer::RemoveObject(ID<Object>& object)
	@brief Remove an `Object`.
	@param [in] object Reference to the `Object`'s `ID`.
	@return `true` if removed. `false` if doesn't exist in `Memory`, or isn't in `Layer`.
*/
auto KTech::Layer::RemoveObject(const ID<Object>& p_object) -> bool
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

/*!
	@brief Remove all contained `Object`s.
	@return `true` if removed all `Object`s. `false` if there are no `Object`s in `Layer`.
*/
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

/*!
	@fn Layer::EnterMap
	@brief Enter a `Map`.
	@param [in] map The `Map` to enter.
	@return `true` if entered `Map`. `false` if given `Map` doesn't exist in `Memory` or already the parent `Map`.
*/
auto KTech::Layer::EnterMap(const ID<Map>& p_map) -> bool
{
	if (p_map == m_parentMap || !engine.memory.maps.Exists(p_map))
	{
		return false;
	}
	return engine.memory.maps[p_map]->AddLayer(m_id);
}

/*!
	@brief Leave parent `Map`.
	@return `true` if left parent `Map` (`Layer::m_parentMap`). `false` if there's no parent `Map`, or the parent `Map` doesn't exist in `Memory`.
*/
auto KTech::Layer::LeaveMap() -> bool
{
	if (engine.memory.maps.Exists(m_parentMap))
	{
		return engine.memory.maps[m_parentMap]->RemoveLayer(m_id);
	}
	m_parentMap = nullID<Map>;
	return true;
}

/*!
	@brief Virtual function called once each tick.

	You can override this in your inherited class to add whatever functionality you want.

	Called by `Memory::CallOnTicks()`.

	@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

	@see `Memory::CallOnTicks()`
	@see `Output::ShouldRenderThisTick()`
*/
auto KTech::Layer::OnTick() -> bool
{
	return false;
};