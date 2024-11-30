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

#include "../utility/cachingregistry.hpp"
#include "../world/camera.hpp"
#include "../world/layer.hpp"
#include "../world/map.hpp"
#include "../world/object.hpp"
#include "../world/ui.hpp"
#include "../world/widget.hpp"

/*!
	Call the virtual `OnTick()` functions of all registered world structures.

	Normally placed at the start of tick, with the other callback-calling functions of engine components. For example:

	@code{.cpp}
	// Game loop
	while (engine.running)
	{
		// Call various callback-functions
		engine.input.CallCallbacks();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks(); // <- Call `OnTick()` functions of all world structures

		// Graphics...
	}
	@endcode

	This function was placed in `Memory`, because this engine component has the most direct access to all of the world structures. Although, this function could have been technically placed easily anywhere else.
*/
void KTech::Memory::CallOnTicks()
{
	for (UI* ui : uis.m_vec)
	{
		if (ui->OnTick() && !m_changedThisTick)
		{
			m_changedThisTick = true;
		}
	}
	for (Widget* widget : widgets.m_vec)
	{
		if (widget->OnTick() && !m_changedThisTick)
		{
			m_changedThisTick = true;
		}
	}
	for (Map* map : maps.m_vec)
	{
		if (map->OnTick() && !m_changedThisTick)
		{
			m_changedThisTick = true;
		}
	}
	for (Camera* camera : cameras.m_vec)
	{
		if (camera->OnTick() && !m_changedThisTick)
		{
			m_changedThisTick = true;
		}
	}
	for (Layer* layer : layers.m_vec)
	{
		if (layer->OnTick() && !m_changedThisTick)
		{
			m_changedThisTick = true;
		}
	}
	for (Object* object : objects.m_vec)
	{
		if (object->OnTick() && !m_changedThisTick)
		{
			m_changedThisTick = true;
		}
	}
}