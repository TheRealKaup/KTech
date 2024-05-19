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

#include <string>
#include <vector>

struct KTech::Map
{
public:
	Engine& engine;
	ID<Map> m_id;
	std::string m_name;
	std::vector<ID<Camera>> m_cameras = {};
	std::vector<ID<Layer>> m_layers = {};

	size_t m_activeCameraI = -1;

	Map(Engine& engine, const std::string& name = "");
	virtual ~Map();

	bool AddLayer(ID<Layer>& layer);
	bool AddCamera(ID<Camera>& camera, bool asActiveCamera = false);

	bool RemoveLayer(ID<Layer>& layer);
	bool RemoveCamera(ID<Camera>& camera);
	bool RemoveAllLayers();
	bool RemoveAllCameras();

	bool Render();

protected:
	inline virtual bool OnTick() { return false; };

	friend class KTech::Memory;
};