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

/*!
	\brief Collection of `Layer`s and `Camera`s.
	
	`Map`s do not interact with each other, making them a good way to divide your game to levels or parallel dimensions, for instance.
	\ingroup world
*/
class KTech::Map
{
public:
	Engine& engine;
	ID<Map> m_id;
	std::string m_name;
	std::vector<ID<Camera>> m_cameras = {}; //!< `Camera`s, only 1 can be active
	std::vector<ID<Layer>> m_layers = {}; //!< `Layer`s, normally all are active

	size_t m_activeCameraI = -1; //!< Index of the `Camera` that should be active. If bigger than `m_cameras`, no `Camera` is deemed active

	Map(Engine& engine, const std::string& name = "");
	virtual ~Map();

	bool AddLayer(ID<Layer>& layer);
	bool AddCamera(ID<Camera>& camera, bool asActiveCamera = false /*!< Set `m_activeCameraI` to point to this given `Camera``*/);

	bool RemoveLayer(ID<Layer>& layer);
	bool RemoveCamera(ID<Camera>& camera);
	bool RemoveAllLayers();
	bool RemoveAllCameras();

	//! Render using `m_cameras[m_activeCameraI]`
	bool Render();

protected:
	inline virtual bool OnTick() { return false; };

	friend class KTech::Memory;
};