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

#include "../utility/container.hpp" 
#include "../world/camera.hpp"
#include "../world/layer.hpp"
#include "../world/map.hpp"
#include "../world/object.hpp"
#include "../world/ui.hpp"
#include "../world/widget.hpp"

void KTech::Memory::CallOnTicks()
{
	size_t i = 0;
	for (; i < uis.m_size && !callChangedThisTick; i++)
		callChangedThisTick = uis.m_arr[i]->OnTick();
	for (; i < uis.m_size; i++)
		uis.m_arr[i]->OnTick();
	i = 0;
	for (; i < widgets.m_size && !callChangedThisTick; i++)
		callChangedThisTick = widgets.m_arr[i]->OnTick();
	for (; i < widgets.m_size; i++)
		widgets.m_arr[i]->OnTick();
	i = 0;
	for (; i < maps.m_size && !callChangedThisTick; i++)
		callChangedThisTick = maps.m_arr[i]->OnTick();
	for (; i < maps.m_size; i++)
		maps.m_arr[i]->OnTick();
	i = 0;
	for (; i < cameras.m_size && !callChangedThisTick; i++)
		callChangedThisTick = cameras.m_arr[i]->OnTick();
	for (; i < cameras.m_size; i++)
		cameras.m_arr[i]->OnTick();
	i = 0;
	for (; i < layers.m_size && !callChangedThisTick; i++)
		callChangedThisTick = layers.m_arr[i]->OnTick();
	for (; i < layers.m_size; i++)
		layers.m_arr[i]->OnTick();
	i = 0;
	for (; i < objects.m_size && !callChangedThisTick; i++)
		callChangedThisTick = objects.m_arr[i]->OnTick();
	for (; i < objects.m_size; i++)
		objects.m_arr[i]->OnTick();
}