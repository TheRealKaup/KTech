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

KTech::Map::Map(Engine& p_engine, const std::string& p_name)
	: engine(p_engine), m_name(p_name)
{
	engine.memory.maps.Add(this);
};

KTech::Map::~Map()
{
	Output::Log("<Map[" + m_name + "]::~Map()>", RGBColors::red);
	for (ID<Layer>& layer : m_layers)
		if (engine.memory.layers.Exists(layer))
			engine.memory.layers[layer]->m_parentMap = ID<Map>(0, 0);
	for (ID<Camera>& camera : m_cameras)
		if (engine.memory.cameras.Exists(camera))
			engine.memory.cameras[camera]->m_parentMap = ID<Map>(0, 0);
	engine.memory.maps.Remove(m_id);
}

int KTech::Map::AddCamera(ID<Camera>& p_camera, bool p_asActiveCamera)
{
	m_cameras.push_back(p_camera);
	if (p_asActiveCamera)
		m_activeCameraI = m_cameras.size() - 1;
	engine.memory.cameras[p_camera]->m_parentMap = m_id;
	return m_cameras.size() - 1;
}

int KTech::Map::AddLayer(ID<Layer>& p_layer)
{
	m_layers.push_back(p_layer);
	engine.memory.layers[p_layer]->m_parentMap = m_id;
	return m_layers.size() - 1;
}

bool KTech::Map::RemoveLayer(ID<Layer>& p_layer)
{
	for (size_t i = 0; i < m_layers.size(); i++)
	{
		if (m_layers[i] == p_layer)
		{
			m_layers.erase(m_layers.begin() + i);
			return true;
		}
	}
	return false;
}

bool KTech::Map::RemoveCamera(ID<Camera>& p_camera)
{
	for (size_t i = 0; i < m_cameras.size(); i++)
	{
		if (m_cameras[i] == p_camera)
		{
			m_cameras.erase(m_cameras.begin() + i);
			return true;
		}
	}
	return false;
}

bool KTech::Map::Render()
{
	if (m_activeCameraI >= 0 && m_activeCameraI < m_cameras.size())
	{
		engine.memory.cameras[m_cameras[m_activeCameraI]]->Render(m_layers);
		return true;
	}
	return false;
}