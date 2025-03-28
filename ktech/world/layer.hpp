/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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
#include "../utility/rgbacolors.hpp"
#include "../basic/rgba.hpp"

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
	Engine& engine; //!< Parent `Engine`
	const ID<Layer> m_id{ID<Layer>::Unique()}; //!< Personal `ID`.
	std::string m_name; //!< String anme; could be useful in debugging.
	ID<Map> m_parentMap; //!< Parent `Map`.
	std::vector<ID<Object>> m_objects; //!< Contained `Object`s.
	bool m_visible = true; //!< `true`: will be rendered by `Camera`. `false`: won't be.

	uint8_t m_alpha = std::numeric_limits<uint8_t>::max(); //!< Opacity used by `Camera` when rendering contained `Object`s.
	RGBA m_frgba = RGBAColors::transparent; //!< Foreground color added by `Camera` after rendering contained `Object`s.
	RGBA m_brgba = RGBAColors::transparent; //!< Background color added by `Camera` after rendering contained `Object`s.

	Layer(Engine& engine, std::string name = "");
	Layer(Engine& engine, const ID<Map>& parentMap, std::string name = "");
	virtual ~Layer();

	auto operator[](size_t index) -> ID<Object>&;

	auto AddObject(const ID<Object>& object) -> bool;
	auto RemoveObject(const ID<Object>& object) -> bool;
	auto RemoveAllObjects() -> bool;

	auto EnterMap(const ID<Map>& map) -> bool;
	auto LeaveMap() -> bool;

protected:
	virtual auto OnTick() -> bool;

	friend class KTech::Memory;
};