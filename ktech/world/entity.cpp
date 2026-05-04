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
	@brief Define KTech::Entity class member functions
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "entity.hpp"

#include "../engine/engine.hpp"

template <class This>
KTech::Entity<This>::Entity(Engine& p_engine, std::string p_name)
	: m_engine(p_engine), m_name(std::move(p_name))
{
	m_engine.memory.Add(reinterpret_cast<This*>(this));
}

template <class This>
KTech::Entity<This>::~Entity()
{
	m_engine.memory.Remove(m_id);
}

template <class This>
auto KTech::Entity<This>::OnTick() -> bool
{
	return false;
}

template class KTech::Entity<KTech::Camera>;
template class KTech::Entity<KTech::Layer>;
template class KTech::Entity<KTech::Map>;
template class KTech::Entity<KTech::Object>;
template class KTech::Entity<KTech::UI>;
template class KTech::Entity<KTech::Widget>;
template class KTech::Entity<KTech::Invocation>;
