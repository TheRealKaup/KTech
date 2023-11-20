/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 E. Kaufman (AKA Kaup)

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

int Engine::Map::AddCamera(Camera* camera, bool asActiveCamera)
{
	cameras.push_back(camera);
	if (asActiveCamera)
		activeCameraI = cameras.size() - 1;
	return cameras.size() - 1;
}

int Engine::Map::AddLayer(Layer* layer)
{
	layers.push_back(layer);
	return layers.size() - 1;
}

bool Engine::Map::Render() const
{
	if (activeCameraI >= 0 && activeCameraI < cameras.size() && cameras[activeCameraI] != nullptr)
	{
		cameras[activeCameraI]->Render(layers);
		return true;
	}
	return false;
}

bool Engine::Map::RenderReversed() const
{
	if (activeCameraI >= 0 && activeCameraI < cameras.size() && cameras[activeCameraI] != nullptr)
	{
		// cameras[activeCameraI]->RenderReversed(layers);
		return true;
	}
	return false;
}

bool Engine::Map::Draw(Point pos, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) const
{
	if (activeCameraI >= 0 && activeCameraI < cameras.size() && cameras[activeCameraI] != nullptr)
	{
		cameras[0]->Draw(pos, left, top, right, bottom);
		return true;
	}
	return false;
}

void Engine::Map::CallOnTicks() const
{
	if (OnTick)
		OnTick();

	for (size_t l = 0; l < layers.size(); l++)
	{
		if (layers[l]->OnTick)
			layers[l]->OnTick();

		for (size_t o = 0; o < layers[l]->objects.size(); o++)
		{
			if (layers[l]->objects[o]->OnEvent)
				layers[l]->objects[o]->OnEvent(Object::EventType::onTick);
		}
	}
	for (size_t c = 0; c < cameras.size(); c++)
	{
		if (cameras[c]->OnTick)
			cameras[c]->OnTick();
	}
}