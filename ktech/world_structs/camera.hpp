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
#include "../misc/id.hpp"
#include "../basic_structs/point.hpp"
#include "../basic_structs/upoint.hpp"
#include "../basic_structs/cella.hpp"

#include <string>
#include <vector>

struct KTech::Camera
{
	Engine& engine;
	ID<Camera> id;

	ID<Map> parentMap;

	std::string name = "";
	Point pos = Point(0, 0);
	UPoint res = UPoint(10, 10);
	CellA background = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)); // The background to render upon.
	std::vector<std::vector<CellA>> image = {};

	void EnterMap(ID<Map>& map);

	void Render();
	void Render(const std::vector<ID<Layer>>& layers);
	void Resize(UPoint res);

	inline virtual void OnTick() {};

	Camera(Engine& engine, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
	Camera(Engine& engine, ID<Map>& parentMap, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
	~Camera();
};