/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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

/*!
	@file
	@brief Define KTech::Memory members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "memory.hpp"

// IWYU pragma: begin_keep
// clangd fails to recognize that call to `entity->OnTick()` depends on these definitions
#include "../world/camera.hpp"
#include "../world/layer.hpp"
#include "../world/map.hpp"
#include "../world/object.hpp"
#include "../world/ui.hpp"
#include "../world/widget.hpp"
// IWYU pragma: end_keep

void KTech::Memory::CallOnTicks()
{
	std::apply(
		[this](auto&... p_registries) -> void {
			(..., [this]<class T>(CachingRegistry<T>& p_registry) -> void {
				// Note that this must be an index-based for loop (see `CachingRegistry::m_vec`).
				for (size_t i = 0; i < p_registry.m_vec.size(); i++)
				{
					if (p_registry.m_vec[i]->OnTick())
					{
						this->m_changedThisTick = true;
					}
				}
				p_registry.Prune();
			}(p_registries));
		},
		m_registries
	);
}
