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
#include "../utility/id.hpp"

#include <string>
#include <vector>

/*!
	@brief World structure that contains `Layer`s and `Camera`s.
*/
class KTech::Map
{
public:
	Engine& engine;						   //!< Parent `Engine`.
	const ID<Map> m_id{ID<Map>::Unique()}; //!< Personal `ID`.
	std::string m_name;					   //!< String name.
	std::vector<ID<Camera>> m_cameras;	   //!< Contained `Camera`s
	std::vector<ID<Layer>> m_layers;	   //!< Contained `Layer`s

	/*!
		@brief Construct a `Map`.
		@param engine Parent `Engine`.
		@param name String name.
	*/
	Map(Engine& engine, std::string name = "");

	/*!
		@brief Remove all `Layer`s and `Camera`s, then remove itself from `Memory`.
	*/
	virtual ~Map();

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

	/*!
		@brief Remove all contained `Layer`s.
		@return `true` if removed all `Layer`s. `false` if there are no `Layer`s in this `Map`.
	*/
	auto RemoveAllLayers() -> bool;

	/*!
		@brief Remove all contained `Camera`s.
		@return `true` if removed all `Camera`s. `false` if there are no `Camera`s in this `Map`.
	*/
	auto RemoveAllCameras() -> bool;

protected:
	/*!
		@brief Virtual function called once each tick.

		You can override this in your inherited class to add whatever functionality you want.

		Called by `Memory::CallOnTicks()`.

		@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

		@see `Memory::CallOnTicks()`
		@see `Output::ShouldRenderThisTick()`
	*/
	virtual auto OnTick() -> bool;

	friend class KTech::Memory;
};
