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
	@brief Define KTech::Layer members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "layer.hpp"

#include "object.hpp"

KTech::Layer::Layer(Engine& p_engine, std::string p_name)
	: ParentChildEntity<Layer, Map, Object>(p_engine, std::move(p_name))
{}

KTech::Layer::Layer(Engine& p_engine, const ID<Map>& p_parentMap, std::string p_name)
	: ParentChildEntity<Layer, Map, Object>(p_engine, p_parentMap, std::move(p_name))
{}

auto KTech::Layer::operator[](size_t p_index) -> ID<Object>&
{
	return GetChildren<Object>()[p_index];
}

auto KTech::Layer::AddObject(const ID<Object>& p_object) -> bool
{
	return Add(p_object);
}

auto KTech::Layer::RemoveObject(const ID<Object>& p_object) -> bool
{
	return Remove(p_object);
}

auto KTech::Layer::EnterMap(const ID<Map>& p_map) -> bool
{
	return Enter(p_map);
}

auto KTech::Layer::LeaveMap() -> bool
{
	return Leave();
}
