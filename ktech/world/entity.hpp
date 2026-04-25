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
	@brief Define KTech::Entity class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../utility/id.hpp"

#include <string>

/*!
	@brief Base class for entities (world structures).

	@tparam This The entity.
*/
template <class This>
class KTech::Entity
{
public:
	Engine& m_engine;
	const ID<This> m_id{ID<This>::Unique()};
	std::string m_name;

	/*!
		@brief Add this entity to Engine::Memory.

		@param engine Parent Engine
		@param name Entity name
	*/
	Entity(Engine& engine, std::string name = "");

	/*!
		@brief Remove this entity from Engine::Memory.
	*/
	virtual ~Entity();

protected:
	/*!
		@brief Virtual function called once each tick.

		You can override this in your inherited class to add whatever functionality you want.

		Called by `Memory::CallOnTicks()`.

		@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.
	*/
	virtual auto OnTick() -> bool;

	friend class KTech::Memory;
};
