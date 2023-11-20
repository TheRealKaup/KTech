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

int Engine::Layer::AddObject(Object* object)
{
	objects.push_back(object);
	object->parentLayer = this;
	return objects.size() - 1;
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

Engine::Layer::~Layer()
{
	// Reset the `parentLayer` of the objects that have it set to this layer
	for (Object*& obj : objects)
		if (obj->parentLayer == this)
			obj->parentLayer = nullptr;
}