#include "engine.hpp"

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

bool Engine::Map::Render()
{
	if (activeCameraI >= 0 && activeCameraI < cameras.size() && cameras[activeCameraI] != nullptr)
	{
		cameras[activeCameraI]->Render(layers);
		return true;
	}
	return false;
}

bool Engine::Map::Draw(Point pos, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	if (activeCameraI >= 0 && activeCameraI < cameras.size() && cameras[activeCameraI] != nullptr)
	{
		cameras[0]->Draw(pos, left, top, right, bottom);
		return true;
	}
	return false;
}

void Engine::Map::CallOnTicks()
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