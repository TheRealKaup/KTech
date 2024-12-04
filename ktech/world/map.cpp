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

#include "map.hpp"

#include "../utility/rgbcolors.hpp"
#include "camera.hpp"
#include "layer.hpp"
#include "../engine/output.hpp"
#include "../engine/engine.hpp"

/*!
	@fn Map::Map
	@brief Construct a `Map`.
	@param engine Parent `Engine`.
	@param name String name.
*/
KTech::Map::Map(Engine& p_engine, std::string p_name)
	: engine(p_engine), m_name(std::move(p_name))
{
	engine.memory.maps.Add(this);
};

/*!
	@brief Remove all `Layer`s and `Camera`s, then remove itself from `Memory`.
*/
KTech::Map::~Map()
{
	Output::Log("<Map[" + m_name + "]::~Map()>", RGBColors::red);
	RemoveAllLayers();
	RemoveAllCameras();
	engine.memory.maps.Remove(m_id);
}

/*!
	@fn Map::AddLayer
	@brief Add a `Layer`.
	@param layer The `ID` of the `Layer` to add.
	@return `true` if added the `Layer`. `false` if given `Layer` doesn't exist in `Memory`, or already in this `Map`.
*/
auto KTech::Map::AddLayer(const ID<Layer>& p_layer) -> bool
{
	if (!engine.memory.layers.Exists(p_layer))
	{
		return false;
	}
	for (ID<Layer>& layer : m_layers)
	{
		if (layer == p_layer)
		{
			return false;
		}
	}
	engine.memory.layers[p_layer]->m_parentMap = m_id;
	m_layers.push_back(p_layer);
	return true;
}

/*!
	@fn Map::AddCamera
	@brief Add a `Camera`.
	@param camera The `ID` of the `Camera` to add.
	@return `true` if added the `Camera`. `false` if given `Camera` doesn't exist in `Memory`, or already in this `Map`.
*/
auto KTech::Map::AddCamera(const ID<Camera>& p_camera) -> bool
{
	if (!engine.memory.cameras.Exists(p_camera))
	{
		return false;
	}
	for (ID<Camera>& camera : m_cameras)
	{
		if (camera == p_camera)
		{
			return false;
		}
	}
	engine.memory.cameras[p_camera]->m_parentMap = m_id;
	m_cameras.push_back(p_camera);
	return true;
}

/*!
	@fn Map::RemoveLayer
	@brief Remove a `Layer`.
	@param layer The `ID` of the `Layer` to remove.
	@return `true` if removed the `Layer`. `false` if the given `Layer` isn't contained by this `Map`.
*/
auto KTech::Map::RemoveLayer(const ID<Layer>& p_layer) -> bool
{
	for (size_t i = 0; i < m_layers.size(); i++)
	{
		if (m_layers[i] == p_layer)
		{
			if (engine.memory.layers.Exists(m_layers[i]))
			{
				engine.memory.layers[m_layers[i]]->m_parentMap = nullID<Map>;
			}
			m_layers.erase(m_layers.begin() + i);
			return true;
		}
	}
	return false;
}

/*!
	@fn Map::RemoveCamera
	@brief Remove a `Camera`.
	@param camera The `ID` of the `Camera` to remove.
	@return `true` if removed the `Camera`. `false` if the given `Camera` isn't contained by this `Map`.
*/
auto KTech::Map::RemoveCamera(const ID<Camera>& p_camera) -> bool
{
	for (size_t i = 0; i < m_cameras.size(); i++)
	{
		if (m_cameras[i] == p_camera)
		{
			if (engine.memory.cameras.Exists(m_cameras[i]))
			{
				engine.memory.cameras[m_cameras[i]]->m_parentMap = nullID<Map>;
			}
			m_cameras.erase(m_cameras.begin() + i);
			return true;
		}
	}
	return false;
}

/*!
	@brief Remove all contained `Layer`s.
	@return `true` if removed all `Layer`s. `false` if there are no `Layer`s in this `Map`.
*/
auto KTech::Map::RemoveAllLayers() -> bool
{
	if (m_layers.empty())
	{
		return false;
	}
	for (auto & layer : m_layers)
	{
		if (engine.memory.layers.Exists(layer))
		{
			engine.memory.layers[layer]->m_parentMap = nullID<Map>;
		}
	}
	m_layers.clear();
	return true;
}

/*!
	@brief Remove all contained `Camera`s.
	@return `true` if removed all `Camera`s. `false` if there are no `Camera`s in this `Map`.
*/
auto KTech::Map::RemoveAllCameras() -> bool
{
	if (m_cameras.empty())
	{
		return false;
	}
	for (auto & camera : m_cameras)
	{
		if (engine.memory.cameras.Exists(camera))
		{
			engine.memory.cameras[camera]->m_parentMap = nullID<Map>;
		}
	}
	m_cameras.clear();
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
auto KTech::Map::OnTick() -> bool
{
	return false;
};