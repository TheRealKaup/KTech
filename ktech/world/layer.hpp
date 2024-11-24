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
#include "../utility/rgbacolors.hpp"
#include "../basic/rgba.hpp"

#include <limits>
#include <string>
#include <vector>

class KTech::Layer
{
public:
	Engine& engine;
	ID<Layer> m_id;
	std::string m_name;
	ID<Map> m_parentMap;
	std::vector<ID<Object>> m_objects;
	bool m_visible = true;

	uint8_t m_alpha = std::numeric_limits<uint8_t>::max();
	RGBA m_frgba = RGBAColors::transparent;
	RGBA m_brgba = RGBAColors::transparent;

	Layer(Engine& engine, std::string name = "");
	Layer(Engine& engine, ID<Map>& parentMap, std::string name = "");
	virtual ~Layer();

	// Use this refereance temporarily - do not move this reference, it can become stale.
	auto operator[](size_t index) -> ID<Object>&;

	auto AddObject(ID<Object>& object) -> bool;
	auto RemoveObject(ID<Object>& object) -> bool;
	auto RemoveAllObjects() -> bool;

	auto EnterMap(ID<Map>& map) -> bool;
	auto LeaveMap() -> bool;

protected:
	inline virtual bool OnTick() { return false; };

	friend class KTech::Memory;
};