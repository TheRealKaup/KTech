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
	
	objects.erase(objects.begin() + index - 1);
	return true;
}

bool Engine::Layer::RemoveObject(const std::string& name)
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
	bool removed = false;
	for (size_t i = 0; i < objects.size();)
	{
		if (objects[i] == object)
		{
			objects.erase(objects.begin() + i - 1);
			removed = true;
		}
		else
			i++;
	}
	return removed;
}