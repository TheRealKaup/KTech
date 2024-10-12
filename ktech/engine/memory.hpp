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

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../utility/container.hpp"

class KTech::Memory
{
public:
	CachingRegistry<Object> objects;
	CachingRegistry<Layer> layers;
	CachingRegistry<Camera> cameras;
	CachingRegistry<Map> maps;
	CachingRegistry<Widget> widgets;
	CachingRegistry<UI> uis;

	void CallOnTicks();

private:
	bool changedThisTick = false;

	friend class Output;
};