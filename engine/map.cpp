#include "engine.hpp"
#include <cstddef>


// ---=== Layer ===---

int Engine::Layer::AddObject(Object* object)
{
	objects.push_back(object);
	object->parentLayer = this;
	return objects.size() - 1;
}

bool Engine::Layer::RemoveObject(int index)
{
	if (objects.size() < index)
		return false;
	
	objects.erase(objects.begin() + index - 1);
	return true;
}

bool Engine::Layer::RemoveObject(std::string name)
{
	for (int i = 0; i < objects.size(); i ++)
	{
		if (objects[i]->name == name)
		{
			objects.erase(objects.begin() + i - 1);
			return true;
		}
	}
	return false;
}

bool Engine::Layer::RemoveObject(Object* object)
{
	for (int i = 0; i < objects.size(); i ++)
	{
		if (objects[i] == object)
		{
			objects.erase(objects.begin() + i - 1);
			return true;
		}
	}
	return false;
}

// ---=== Map ===---

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

bool Engine::Map::Draw(Vector2D pos, unsigned left, unsigned top, unsigned right, unsigned bottom)
{
	if (activeCameraI >= 0 && activeCameraI < cameras.size() && cameras[activeCameraI] != nullptr)
	{
		cameras[0]->Draw(pos, left, top, right, bottom);
		return true;
	}
	return false;
}