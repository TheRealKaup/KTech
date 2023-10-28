#include "engine.hpp"

int Engine::Layer::AddObject(Object* object)
{
	objects.push_back(object);
	object->parentLayer = this;
	return objects.size() - 1;
}

bool Engine::Layer::RemoveObject(size_t index)
{
	if (objects.size() < index)
		return false;
	
	objects[index]->parentLayer = nullptr;
	objects.erase(objects.begin() + index);
	return true;
}

bool Engine::Layer::RemoveObject(const std::string& name)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i]->name == name)
		{
			objects[i]->parentLayer = nullptr;
			objects.erase(objects.begin() + i);
			return true;
		}
	}
	return false;
}

bool Engine::Layer::RemoveObject(Object* object)
{
	bool removed = false;
	for (size_t i = 0; i < objects.size();)
	{
		if (objects[i] == object)
		{
			objects[i]->parentLayer = nullptr;
			objects.erase(objects.begin() + i);
			removed = true;
		}
		else
			i++;
	}
	return removed;
}