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

KTech::Map::Map(Engine& p_engine, std::string p_name)
	: engine(p_engine), m_name(std::move(p_name))
{
	engine.memory.maps.Add(this);
};

KTech::Map::~Map()
{
	Output::Log("<Map[" + m_name + "]::~Map()>", RGBColors::red);
	RemoveAllLayers();
	RemoveAllCameras();
	engine.memory.maps.Remove(m_id);
}

auto KTech::Map::AddLayer(ID<Layer>& p_layer) -> bool
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

auto KTech::Map::AddCamera(ID<Camera>& p_camera, bool p_asActiveCamera) -> bool
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
	if (p_asActiveCamera)
	{
		m_activeCameraI = m_cameras.size() - 1;
	}
	return true;
}

auto KTech::Map::RemoveLayer(ID<Layer>& p_layer) -> bool
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

auto KTech::Map::RemoveCamera(ID<Camera>& p_camera) -> bool
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

bool KTech::Map::RemoveAllLayers()
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

auto KTech::Map::Render() -> bool
{
	if (m_activeCameraI >= 0 && m_activeCameraI < m_cameras.size())
	{
		engine.memory.cameras[m_cameras[m_activeCameraI]]->Render(m_layers);
		return true;
	}
	return false;
}