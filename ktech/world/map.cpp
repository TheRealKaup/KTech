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
#include "layer.hpp"
#include "camera.hpp"
#include "../engine/engine.hpp"

int KTech::Map::AddCamera(ID<Camera>& camera, bool asActiveCamera)
{
	cameras.push_back(camera);
	if (asActiveCamera)
		activeCameraI = cameras.size() - 1;
	engine.memory.cameras[camera]->parentMap = id;
	return cameras.size() - 1;
}

int KTech::Map::AddLayer(ID<Layer>& layer)
{
	layers.push_back(layer);
	engine.memory.layers[layer]->parentMap = id;
	return layers.size() - 1;
}

bool KTech::Map::Render()
{
	if (activeCameraI >= 0 && activeCameraI < cameras.size())
	{
		engine.memory.cameras[cameras[activeCameraI]]->Render(layers);
		return true;
	}
	return false;
}

bool KTech::Map::RemoveLayer(ID<Layer>& layer)
{
	for (size_t i = 0; i < layers.size(); i++)
	{
		if (layers[i] == layer)
		{
			layers.erase(layers.begin() + i);
			return true;
		}
	}
	return false;
}

bool KTech::Map::RemoveCamera(ID<Camera>& camera)
{
	for (size_t i = 0; i < cameras.size(); i++)
	{
		if (cameras[i] == camera)
		{
			cameras.erase(cameras.begin() + i);
			return true;
		}
	}
	return false;
}

KTech::Map::Map(Engine& engine)
	: engine(engine)
{
	engine.memory.maps.Add(this);
};

KTech::Map::~Map()
{
	IO::Log("<Map[" + name + "]::~Map()>", RGBColors::red);
	for (ID<Layer>& layer : layers)
		if (engine.memory.layers.Exists(layer))
			engine.memory.layers[layer]->parentMap = ID<Map>(0, 0);
	for (ID<Camera>& camera : cameras)
		if (engine.memory.cameras.Exists(camera))
			engine.memory.cameras[camera]->parentMap = ID<Map>(0, 0);
	engine.memory.maps.Remove(id);
}