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
	@brief Define KTech::Layer class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../basic/rgba.hpp"
#include "../utility/id.hpp"
#include "../utility/rgbacolors.hpp"

#include <limits>
#include <string>
#include <vector>

/*!
	@brief World structure that contains `Object`s, and exists within `Map`.

	Separates `Object`s in collision (only `Object`s from the same `Layer` can collide), and orders `Object`s in rendering (`Object`s in the first `Layer` added to a `Map` will be covered by `Object`s from the following `Layer`, and so on).
*/
class KTech::Layer
{
public:
	Engine& engine;							   //!< Parent `Engine`
	const ID<Layer> m_id{ID<Layer>::Unique()}; //!< Personal `ID`.
	std::string m_name;						   //!< String anme; could be useful in debugging.
	ID<Map> m_parentMap;					   //!< Parent `Map`.
	std::vector<ID<Object>> m_objects;		   //!< Contained `Object`s.
	bool m_visible = true;					   //!< `true`: will be rendered by `Camera`. `false`: won't be.

	//! Opacity used by `Camera` when rendering contained `Object`s.
	uint8_t m_alpha = std::numeric_limits<uint8_t>::max();
	RGBA m_frgba = RGBAColors::transparent; //!< Foreground color added by `Camera` after rendering contained `Object`s.
	RGBA m_brgba = RGBAColors::transparent; //!< Background color added by `Camera` after rendering contained `Object`s.

	/*!
		@fn Layer::Layer(Engine& engine, std::string name)
		@brief Construct a `Layer`.
		@param [in] engine Parent `Engine`.
		@param [in] name String name.
	*/
	Layer(Engine& engine, std::string name = "");

	/*!
		@fn Layer::Layer(Engine& engine, const ID<Map>& parentMap, std::string name)
		@brief Construct a `Layer` and immediately enter a `Map`.
		@param [in] engine Parent `Engine`.
		@param [in] parentMap `Map` to enter.
		@param [in] name String name.
	*/
	Layer(Engine& engine, const ID<Map>& parentMap, std::string name = "");

	/*!
		@brief Destruct a `Layer`.

		Removes all contained `Object`s, leaves parent `Map` (if in one), and removes itself from `Memory`.
	*/
	virtual ~Layer();

	/*!
		@brief Retrieve `Object` `ID` at given index.

		@param index The index of the Object ID in m_objects

		@return Reference to the `ID<Object>` at the given index.
	*/
	auto operator[](size_t index) -> ID<Object>&;

	/*!
		@fn Layer::AddObject(ID<Object>& object)
		@brief Add an `Object`.
		@param [in] object The `Object` to add.
		@return `true` if added the `Object`. `false` if `Object` doesn't exist in `Memory` or already contained by `Layer`.
	*/
	auto AddObject(const ID<Object>& object) -> bool;

	/*!
		@fn Layer::RemoveObject(ID<Object>& object)
		@brief Remove an `Object`.
		@param [in] object Reference to the `Object`'s `ID`.
		@return `true` if removed. `false` if doesn't exist in `Memory`, or isn't in `Layer`.
	*/
	auto RemoveObject(const ID<Object>& object) -> bool;

	/*!
		@brief Remove all contained `Object`s.
		@return `true` if removed all `Object`s. `false` if there are no `Object`s in `Layer`.
	*/
	auto RemoveAllObjects() -> bool;

	/*!
		@fn Layer::EnterMap
		@brief Enter a `Map`.
		@param [in] map The `Map` to enter.
		@return `true` if entered `Map`. `false` if given `Map` doesn't exist in `Memory` or already the parent `Map`.
	*/
	auto EnterMap(const ID<Map>& map) -> bool;

	/*!
		@brief Leave parent `Map`.
		@return `true` if left parent `Map` (`Layer::m_parentMap`). `false` if there's no parent `Map`, or the parent `Map` doesn't exist in `Memory`.
	*/
	auto LeaveMap() -> bool;

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
