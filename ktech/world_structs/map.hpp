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

#include <string>
#include <vector>

struct KTech::Map
{
public:
	Engine& engine;
	ID<Map> id;

	std::string name = "";
	std::vector<ID<Camera>> cameras = {};
	std::vector<ID<Layer>> layers = {};
	size_t activeCameraI = -1;

	int AddLayer(ID<Layer>& layer);
	int AddCamera(ID<Camera>& camera, bool asActiveCamera = false);

	bool RemoveLayer(ID<Layer>& layer);
	bool RemoveCamera(ID<Camera>& camera);

	bool Render();

	inline virtual void OnTick() {};
	
	Map(Engine& engine);
	~Map();
};