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
	@brief Define KTech::CachingRegistry class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "cachingregistry.hpp"

#include "../world/camera.hpp"
#include "../world/layer.hpp"
#include "../world/map.hpp"
#include "../world/object.hpp"
#include "../world/ui.hpp"
#include "../world/widget.hpp"

template <typename T>
auto KTech::CachingRegistry<T>::operator[](const ID<T>& p_id) -> T*
{
	return UpdateID(p_id) ? m_vec[p_id.m_i] : nullptr;
}

template <typename T>
void KTech::CachingRegistry<T>::Add(T* p_structure)
{
	p_structure->m_id.m_i = m_vec.size();
	m_vec.push_back(p_structure);
}

template <typename T>
void KTech::CachingRegistry<T>::Remove(const ID<T>& p_id)
{
	if (UpdateID(p_id))
	{
		m_vec[p_id.m_i] = nullptr;
	}
}

template <typename T>
auto KTech::CachingRegistry<T>::UpdateID(const ID<T>& p_id) -> bool
{
	if (!m_vec.empty())
	{
		for (p_id.m_i = std::min(p_id.m_i, m_vec.size() - 1);; p_id.m_i--)
		{
			if ((m_vec[p_id.m_i] != nullptr) && (m_vec[p_id.m_i]->m_id == p_id))
			{
				return true;
			}
			if (p_id.m_i == 0)
			{
				break;
			}
		}
	}
	return false;
}

template <typename T>
void KTech::CachingRegistry<T>::Prune()
{
	std::erase(m_vec, nullptr);
}

template class KTech::CachingRegistry<KTech::Camera>;
template class KTech::CachingRegistry<KTech::Layer>;
template class KTech::CachingRegistry<KTech::Map>;
template class KTech::CachingRegistry<KTech::Object>;
template class KTech::CachingRegistry<KTech::UI>;
template class KTech::CachingRegistry<KTech::Widget>;
