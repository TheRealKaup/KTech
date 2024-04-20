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

struct KTech::Layer
{
	Engine& engine;
	ID<Layer> m_id;
	std::string m_name;
	ID<Map> m_parentMap;
	std::vector<ID<Object>> m_objects;
	bool m_visible = true;

	uint8_t m_alpha = 255;
	RGBA m_frgba = RGBA(0, 0, 0, 0);
	RGBA m_brgba = RGBA(0, 0, 0, 0);

	Layer(Engine& engine, const std::string& name = "");
	Layer(Engine& engine, ID<Map>& parentMap, const std::string& name = "");
	~Layer();

	// Use this refereance temporarily - do not move this reference, it can become stale.
	ID<Object>& operator[](size_t index);
	
	inline virtual void OnTick() {};

	void AddObject(ID<Object>& object);
	
	bool RemoveObject(const std::string& name);
	bool RemoveObject(ID<Object>& object);

	void EnterMap(ID<Map>& map);
};