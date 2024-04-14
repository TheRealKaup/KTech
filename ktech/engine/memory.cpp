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

#include "memory.hpp"
#include "../world_structs/ui.hpp"
#include "../world_structs/widget.hpp"
#include "../world_structs/map.hpp"
#include "../world_structs/camera.hpp"
#include "../world_structs/layer.hpp"
#include "../world_structs/object.hpp"
#include "../misc/container.hpp"

void KTech::Memory::CallOnTicks()
{
	for (size_t i = 0; i < uis.size; i++)
		uis.arr[i]->OnTick();
	for (size_t i = 0; i < widgets.size; i++)
		widgets.arr[i]->OnTick();
	for (size_t i = 0; i < maps.size; i++)
		maps.arr[i]->OnTick();
	for (size_t i = 0; i < cameras.size; i++)
		cameras.arr[i]->OnTick();
	for (size_t i = 0; i < layers.size; i++)
		layers.arr[i]->OnTick();
	for (size_t i = 0; i < objects.size; i++)
		objects.arr[i]->OnTick();
}