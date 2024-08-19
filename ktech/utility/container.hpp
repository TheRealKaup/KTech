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

#include <cstddef>

template<typename T>
struct KTech::Container
{
	// The structures are stored as generic identifiables, which contain UUIDs.
	T** m_arr = nullptr;
	// The size of the array
	size_t m_size = 0;

	// Takes an ID reference, returns the pointer to the structure.
	// If the ID is outdated (made so by structures being removed) then update it.
	// If the ID's UUID doesn't exist in the storage at all, this will reset the ID and will return `nullptr`.
	T* operator[](ID<T>& p_id)
	{
		if (m_size > 0)
		{
			for (size_t i = (p_id.m_i < m_size ? p_id.m_i : m_size - 1);; i--)
			{
				if (m_arr[i]->m_id == p_id)
				{
					p_id.m_i = i;
					return m_arr[i];
				}
				if (i == 0)
					break;
			}
		}
		p_id.m_i = 0;
		return nullptr;
	}
	// Takes an ID reference, returns the pointer to the structure.
	// Constant version of the previous operator, which will not update the given ID if it's outdated/missing.
	T* operator[](const ID<T>& p_id)
	{
		if (m_size > 0)
		{
			for (size_t i = (p_id.m_i < m_size ? p_id.m_i : m_size -1);; i--)
			{
				if (m_arr[i]->m_id == p_id)
					return m_arr[i];
				if (i == 0)
					break;
			}
		}
		return nullptr;
	}

	// Adds the pointer to the container.
	// Automatically called by objects, layers, cameras and maps for themselves.
	// You shouldn't call this manually on a structure.
	ID<T> Add(T* structure)
	{
		T** tmp = new T*[m_size + 1];
		for (size_t i = 0; i < m_size; i++)
			tmp[i] = m_arr[i];
		structure->m_id.m_i = m_size;
		tmp[m_size] = structure;
		if (m_size > 0)
		{
			delete[] m_arr;
		}
		m_arr = tmp;
		m_size++;
		return m_arr[m_size - 1]->m_id;
	}

	// Remove a structure from storage (doesn't delete it's memory).
	// Returns true if the structure was found and removed.
	// Returns false if the structure is missing.
	bool Remove(const ID<T>& id)
	{
		size_t toRemove = IDToIndex(id);
		if (toRemove == m_size)
		{
			return false;
		}
		T** tmp = new T*[m_size - 1];
		size_t i = 0;
		for (; i < toRemove; i++)
			tmp[i] = m_arr[i];
		for (i++; i < m_size; i++)
			tmp[i - 1] = m_arr[i];
		delete[] m_arr;
		m_size--;
		m_arr = tmp;
		return true;
	}

	// Returns true if the structure is found, false if missing.
	// Fixes the ID if outdated.
	bool Exists(ID<T>& id)
	{
		if (IDToIndex(id) == m_size)
			return false;
		return true;
	}
	// Returns true if the structure is found, false if missing.
	bool Exists(const ID<T>& id)
	{
		if (IDToIndex(id) == m_size)
			return false;
		return true;
	}
	// Returns the valid index of the ID.
	// If the UUID is missing, return the size of the array making the index invalid.
	// Fixes the ID if outdated.
	size_t IDToIndex(ID<T>& id)
	{
		if (m_size > 0)
		{
			for (size_t i = (id.m_i < m_size ? id.m_i : m_size - 1);; i--)
			{
				if (m_arr[i]->m_id == id)
				{	
					id.m_i = i;
					return i;
				}
				if (i == 0)
					break;
			}
		}
		id.m_i = 0;
		return m_size;
	}
	// Returns the valid index of the ID.
	// If the UUID is missing, return the size of the array making the index invalid.
	size_t IDToIndex(const ID<T>& id)
	{
		if (m_size > 0)
		{
			for (size_t i = (id.m_i < m_size ? id.m_i : m_size - 1);; i--)
			{
				if (m_arr[i]->m_id == id)
					return i;
				if (i == 0)
					break;
			}
		}
		return m_size;
	}
};