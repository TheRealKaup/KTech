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

#include "ktech.hpp"

void KTech::Object::EnterLayer(ID<Layer>& layer)
{
	if (engine.memory.layers.Exists(parentLayer))
		engine.memory.layers[parentLayer]->RemoveObject(id);
	engine.memory.layers[layer]->AddObject(id);
}

bool KTech::Object::Move(Point dir)
{
	// Request the collision manager of the engine to move this object.
	return engine.collision.MoveObject(id, dir);
}

KTech::Object::Object(Engine& engine, Point pos, const std::string& name)
	: engine(engine), pos(pos), name(name)
{
	engine.memory.objects.Add(this);
}

KTech::Object::Object(Engine& engine, ID<Layer>& layer, Point pos, const std::string& name)
	: engine(engine), pos(pos), name(name)
{
	engine.memory.objects.Add(this);
	EnterLayer(layer);
}

KTech::Object::~Object()
{
	IO::Log("<Object[" + name + ", " + std::to_string((size_t)this) + "]::~Object()> Start of function...", RGBColors::red);
	if (engine.memory.layers.Exists(parentLayer))
	{
		IO::Log("<Object[" + name + "]::~Object()> Parent layer stil exists, remove this object from it", RGBColors::red);
			engine.memory.layers[parentLayer]->RemoveObject(id);
	}
	IO::Log("<Object[" + name + "]::~Object()> Remove this object from memory", RGBColors::red);
	engine.memory.objects.Remove(id);
	IO::Log("<Object[" + name + "]::~Object()> End of function.", RGBColors::red);
}