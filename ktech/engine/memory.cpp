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
	for (; i < uis.m_vec.size() && !changedThisTick; i++)
		changedThisTick = uis.m_vec[i]->OnTick();
	for (; i < uis.m_vec.size(); i++)
		uis.m_vec[i]->OnTick();
	i = 0;
	for (; i < widgets.m_vec.size() && !changedThisTick; i++)
		changedThisTick = widgets.m_vec[i]->OnTick();
	for (; i < widgets.m_vec.size(); i++)
		widgets.m_vec[i]->OnTick();
	i = 0;
	for (; i < maps.m_vec.size() && !changedThisTick; i++)
		changedThisTick = maps.m_vec[i]->OnTick();
	for (; i < maps.m_vec.size(); i++)
		maps.m_vec[i]->OnTick();
	i = 0;
	for (; i < cameras.m_vec.size() && !changedThisTick; i++)
		changedThisTick = cameras.m_vec[i]->OnTick();
	for (; i < cameras.m_vec.size(); i++)
		cameras.m_vec[i]->OnTick();
	i = 0;
	for (; i < layers.m_vec.size() && !changedThisTick; i++)
		changedThisTick = layers.m_vec[i]->OnTick();
	for (; i < layers.m_vec.size(); i++)
		layers.m_vec[i]->OnTick();
	i = 0;
	for (; i < objects.m_vec.size() && !changedThisTick; i++)
		changedThisTick = objects.m_vec[i]->OnTick();
	for (; i < objects.m_vec.size(); i++)
		objects.m_vec[i]->OnTick();
}