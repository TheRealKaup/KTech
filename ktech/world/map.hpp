/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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

/*!
	@file
	@brief Define KTech::Map class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "entity.hpp"

/*!
	@brief World structure that contains `Layer`s and `Camera`s.
*/
class KTech::Map : public KTech::ParentEntity<Map, Camera, Layer>
{
public:
	using ParentEntity::ParentEntity;

	/*!
		@brief Construct a `Map`.
		@param engine Parent `Engine`.
		@param name String name.
	*/
	Map(Engine& engine, std::string name = "");

	/*!
		@brief Add a `Layer`.
		@param layer The `ID` of the `Layer` to add.
		@return `true` if added the `Layer`. `false` if given `Layer` doesn't exist in `Memory`, or already in this `Map`.
	*/
	auto AddLayer(const ID<Layer>& layer) -> bool;

	/*!
		@brief Add a `Camera`.
		@param camera The `ID` of the `Camera` to add.
		@return `true` if added the `Camera`. `false` if given `Camera` doesn't exist in `Memory`, or already in this `Map`.
	*/
	auto AddCamera(const ID<Camera>& camera) -> bool;

	/*!
		@brief Remove a `Layer`.
		@param layer The `ID` of the `Layer` to remove.
		@return `true` if removed the `Layer`. `false` if the given `Layer` isn't contained by this `Map`.
	*/
	auto RemoveLayer(const ID<Layer>& layer) -> bool;

	/*!
		@brief Remove a `Camera`.
		@param camera The `ID` of the `Camera` to remove.
		@return `true` if removed the `Camera`. `false` if the given `Camera` isn't contained by this `Map`.
	*/
	auto RemoveCamera(const ID<Camera>& camera) -> bool;
};
