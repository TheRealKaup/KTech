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
#include "../basic/cella.hpp"
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"

#include <string>
#include <vector>

struct KTech::Camera
{
	Engine& engine;
	ID<Camera> m_id;
	std::string m_name;
	ID<Map> m_parentMap;

	Point m_pos;
	UPoint m_res;
	CellA m_background = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)); // The background to render upon.
	std::vector<std::vector<CellA>> m_image = {};

	Camera(Engine& engine, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
	Camera(Engine& engine, ID<Map>& parentMap, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
	~Camera();

	inline virtual void OnTick() {};
	
	void EnterMap(ID<Map>& map);

	void Resize(UPoint resolution); // Will also update `m_image`.

	void Render(); // Will render the layers in `m_parentMap`
	void Render(const std::vector<ID<Layer>>& layers);
};