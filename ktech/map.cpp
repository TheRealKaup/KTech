/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 Ethan Kaufman (AKA Kaup)

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

#include "ktech.hpp"

int KTech::Map::AddCamera(ID<Camera>& camera, bool asActiveCamera)
{
	cameras.push_back(camera);
	if (asActiveCamera)
		activeCameraI = cameras.size() - 1;
	return cameras.size() - 1;
}

int KTech::Map::AddLayer(ID<Layer>& layer)
{
	KTech::IO::Log("<Map::AddLayer()> Start of function...", RGBColors::mint);
	engine.memory.layers[layer]->parentMap = id;
	layers.push_back(layer);
	KTech::IO::Log("<Map::AddLayer()> End of function.", RGBColors::mint);
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
	engine.memory.maps.Remove(id);	
}