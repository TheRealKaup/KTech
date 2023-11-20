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

void Engine::Object::EnterLayer(Engine::Layer* layer)
{
	if (parentLayer != nullptr)
		parentLayer->RemoveObject(this);
	layer->AddObject(this);
}

Engine::Object::Object(Point pos, Layer* layer, const std::string& name)
	: pos(pos), name(name)
{
	if(layer)
		EnterLayer(layer);
}

Engine::Object::~Object()
{
	if (parentLayer)
		parentLayer->RemoveObject(this);
}