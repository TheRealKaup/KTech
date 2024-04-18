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

#include "container.hpp"
#include "../utility/rgbcolors.hpp"
#include "../world/camera.hpp"
#include "../world/layer.hpp"
#include "../world/map.hpp"
#include "../world/object.hpp"
#include "../world/ui.hpp"
#include "../world/widget.hpp"
#include "../engine/output.hpp"

// Explicit instantiation
template class KTech::Container<KTech::Object>;
template class KTech::Container<KTech::Layer>;
template class KTech::Container<KTech::Camera>;
template class KTech::Container<KTech::Map>;
template class KTech::Container<KTech::Widget>;
template class KTech::Container<KTech::UI>;

template<typename T>
T* KTech::Container<T>::operator[](ID<T>& p_id)
{
	for (size_t i = (p_id.m_i < m_size ? p_id.m_i : m_size -1);; i--)
	{
		if (m_arr[i]->m_id == p_id)
		{
			p_id.m_i = i;
			return m_arr[i];
		}
		if (i == 0)
			break;
	}
	p_id.m_i = 0;
	return nullptr;
}

template<typename T>
T* KTech::Container<T>::operator[](const ID<T>& p_id)
{
	for (size_t i = (p_id.m_i < m_size ? p_id.m_i : m_size -1);; i--)
	{
		if (m_arr[i]->m_id == p_id)
			return m_arr[i];
		if (i == 0)
			break;
	}
	return nullptr;
}

template<typename T>
KTech::ID<T> KTech::Container<T>::Add(T* p_structure)
{
	KTech::Output::Log("<Container::Add()> Start of function...", RGBColors::lime);
	T** tmp = new T*[m_size + 1];
	KTech::Output::Log("<Container::Add()> Created new extended array " + std::to_string((size_t)tmp), RGBColors::lime);
	KTech::Output::Log("<Container::Add()> Move to new array", RGBColors::lime);
	for (size_t i = 0; i < m_size; i++)
		tmp[i] = m_arr[i];
	KTech::Output::Log("<Container::Add()> Add given structure", RGBColors::lime);
	p_structure->m_id.m_i = m_size;
	tmp[m_size] = p_structure;
	if (m_size > 0)
	{
		KTech::Output::Log("<Container::Add()> Delete old array " + std::to_string((size_t)m_arr), RGBColors::lime);
		delete[] m_arr;
	}
	KTech::Output::Log("<Container::Add()> Update info", RGBColors::lime);
	m_arr = tmp;
	m_size++;
	KTech::Output::Log("<Container::Add()> End of function, returning ID.", RGBColors::lime);
	return m_arr[m_size - 1]->m_id;
}

template<typename T>
inline bool KTech::Container<T>::Remove(const ID<T>& p_id)
{
	KTech::Output::Log("<Container::Remove()> Start of function...", RGBColors::lime);
	KTech::Output::Log("<Container::Remove()> Convert given ID to index", RGBColors::lime);
	size_t toRemove = IDToIndex(p_id);
	if (toRemove == m_size)
	{
		KTech::Output::Log("<Container::Remove()> End of function.", RGBColors::lime);
		return false;
	}
	T** tmp = new T*[m_size - 1];
	KTech::Output::Log("<Container::Remove()> Created new smaller array " + std::to_string((size_t)tmp), RGBColors::lime);
	KTech::Output::Log("<Container::Remove()> Moving to new array", RGBColors::lime);
	size_t i = 0;
	for (; i < toRemove; i++)
		tmp[i] = m_arr[i];
	for (i++; i < m_size; i++)
		tmp[i - 1] = m_arr[i];
	KTech::Output::Log("<Container::Remove()> Delete old array " + std::to_string((size_t)m_arr), RGBColors::lime);
	delete[] m_arr;
	KTech::Output::Log("<Container::Remove()> Update info", RGBColors::lime);
	m_size--;
	m_arr = tmp;
	KTech::Output::Log("<Container::Remove()> End of function.", RGBColors::lime);
	return true;
}

template<typename T>
bool KTech::Container<T>::Exists(const ID<T>& p_id)
{
	if (IDToIndex(p_id) == m_size)
		return false;
	return true;
}

template<typename T>
bool KTech::Container<T>::Exists(ID<T>& p_id)
{
	if (IDToIndex(p_id) == m_size)
		return false;
	return true;
}

template<typename T>
inline size_t KTech::Container<T>::IDToIndex(const ID<T>& p_id)
{
	for (size_t i = (p_id.m_i < m_size ? p_id.m_i : m_size - 1);; i--)
	{
		if (m_arr[i]->m_id == p_id)
			return i;
		if (i == 0)
			return m_size;
	}
}

template<typename T>
inline size_t KTech::Container<T>::IDToIndex(ID<T>& p_id)
{
	for (size_t i = (p_id.m_i < m_size ? p_id.m_i : m_size - 1);; i--)
	{
		if (m_arr[i]->m_id == p_id)
		{	
			p_id.m_i = i;
			return i;
		}
		if (i == 0)
			break;
	}
	p_id.m_i = 0;
	return m_size;
}