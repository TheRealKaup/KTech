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
#include "../utility/id.hpp"
#include "../basic/rgba.hpp"

#include <string>
#include <vector>

/*!
	\brief Collection of `Object`s.

	\ingroup world
*/
class KTech::Layer
{
public:
	Engine& engine;
	ID<Layer> m_id;
	std::string m_name;
	ID<Map> m_parentMap;
	std::vector<ID<Object>> m_objects; //!< `Object`s
	bool m_visible = true; //!< Determines whether to render itself and its `Object`s

	uint8_t m_alpha = 255; //!< Additional alpha channel for its `Object`s
	RGBA m_frgba = RGBA(0, 0, 0, 0); //!< Added foreground color (after its `Object`s are rendered)
	RGBA m_brgba = RGBA(0, 0, 0, 0); //!< Added background color (after its `Object`s are rendered)

	Layer(Engine& engine, const std::string& name = "");
	Layer(Engine& engine, ID<Map>& parentMap, const std::string& name = "");
	virtual ~Layer();

	ID<Object>& operator[](size_t index);
	
	bool AddObject(ID<Object>& object);
	bool RemoveObject(ID<Object>& object);
	bool RemoveAllObjects();

	bool EnterMap(ID<Map>& map);
	bool LeaveMap();

protected:
	inline virtual bool OnTick() { return false; };

	friend class KTech::Memory;
};