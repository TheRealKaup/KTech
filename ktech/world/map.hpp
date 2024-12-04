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
	World structure that contains `Layer`s and `Camera`s.
*/
class KTech::Map
{
public:
	Engine& engine; //!< Parent `Engine`.
	const ID<Map> m_id{ID<Map>::Unique()}; //!< Personal `ID`.
	std::string m_name; //!< String name.
	std::vector<ID<Camera>> m_cameras = {}; //!< Contained `Camera`s
	std::vector<ID<Layer>> m_layers = {}; //!< Contained `Layer`s

	Map(Engine& engine, std::string name = "");
	virtual ~Map();

	auto AddLayer(const ID<Layer>& layer) -> bool;
	auto AddCamera(const ID<Camera>& camera) -> bool;

	auto RemoveLayer(const ID<Layer>& layer) -> bool;
	auto RemoveCamera(const ID<Camera>& camera) -> bool;
	auto RemoveAllLayers() -> bool;
	auto RemoveAllCameras() -> bool;

protected:
	virtual auto OnTick() -> bool;

	friend class KTech::Memory;
};