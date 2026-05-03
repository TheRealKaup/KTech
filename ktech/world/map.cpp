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
	@brief Define KTech::Map members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "map.hpp"

#include "../engine/engine.hpp"
#include "../engine/output.hpp"
#include "../utility/rgbcolors.hpp"
#include "camera.hpp"
#include "layer.hpp"

KTech::Map::Map(Engine& p_engine, std::string p_name)
	: Entity(p_engine, std::move(p_name))
{}

KTech::Map::~Map()
{
	Output::Log("<Map[" + m_name + "]::~Map()>", RGBColors::red);
	RemoveAllLayers();
	RemoveAllCameras();
}

auto KTech::Map::AddLayer(const ID<Layer>& p_layer) -> bool
{
	return m_layers.Add(m_engine, m_id, p_layer, &Layer::m_parentMap, &Layer::LeaveMap);
}

auto KTech::Map::AddCamera(const ID<Camera>& p_camera) -> bool
{
	return m_cameras.Add(m_engine, m_id, p_camera, &Camera::m_parentMap, &Camera::LeaveMap);
}

auto KTech::Map::RemoveLayer(const ID<Layer>& p_layer) -> bool
{
	return m_layers.Remove(m_engine, p_layer, &Layer::m_parentMap);
}

auto KTech::Map::RemoveCamera(const ID<Camera>& p_camera) -> bool
{
	return m_cameras.Remove(m_engine, p_camera, &Camera::m_parentMap);
}

auto KTech::Map::RemoveAllLayers() -> bool
{
	return m_layers.RemoveAll(m_engine, &Layer::m_parentMap);
}

auto KTech::Map::RemoveAllCameras() -> bool
{
	return m_cameras.RemoveAll(m_engine, &Camera::m_parentMap);
}
