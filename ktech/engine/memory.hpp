/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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
#include "../utility/cachingregistry.hpp"

/*!
	@brief Engine component responsible for registering all world structures.

	This engine component contains a `CachingRegistry` for each world structure. World structures automatically add themselves to their corresponding `CachingRegistry` here. This way world structures can refer and get access to each other in a serializable way, using their `ID`s (in contrary to a memory addresses, which aren't serializable nor safe in terms of memory). This is why world structures need an `Engine` reference (and is indeed a non-optional parameter in all of their constructors); they must have access to this place to integrate with everything else.

	@see `CachingRegistry` to learn about accessing world structures using their `ID`.
*/
class KTech::Memory
{
public:
	CachingRegistry<Object> objects; //!< `Object`s registry.
	CachingRegistry<Layer> layers; //!< `Layer`s registry.
	CachingRegistry<Camera> cameras; //!< `Camera`s registry.
	CachingRegistry<Map> maps; //!< `Map`s registry.
	CachingRegistry<Widget> widgets; //!< `Widget`s registry.
	CachingRegistry<UI> uis; //!< `UI`s registry.

	void CallOnTicks();

private:
	bool m_changedThisTick = false;

	friend class Output;
};