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

#include <cstddef>
#include <vector>

/*!
	@brief Registry that retrieves world structure pointers by serializable `ID`.

	From the "How does `CachingRegistry` work?" `faq.md` entry:

	> `KTech::ID` is initialized with a UUID value when constructed, meaning, references to world structures (classes that have an `ID m_id` member) are serializable. All that is left is some kind of a container to store world structure instances or register world structure pointers, so world structures can be retrieved using their `ID`s.
	>
	> I chose to register pointers rather than store instances, and `KTech::CachingRegistry` is implementation. Its purpose is to retrieve a registered world structure pointer, given a corresponding UUID. As its name suggests, it doesn't simply iterate through all registered world structures and retrieves the one that has the matching UUID, but rather, it uses some kind of caching-related optimization.
	>
	> `KTech::ID` doesn't only comprise a UUID value: it also has an index (`ID::m_i`). This member is used by `CachingRegistry` to cache the last known index in the registry of the associated world structure. When `CachingRegistry` attempts to retrieve a world structure pointer, it first checks if the UUID of the given `ID` matches the UUID of the world structure registered at the given cached index. Optimally they match, so it simply returns the pointer from the cached index. If they don't, it iterates down the registry until it finds the matching UUID (i.e. the wanted world structure).
	>
	> `CachingRegistry` will update the cached index of any `ID` it receives, expediting future usages of that `ID`. The cached index of `ID` is declared `mutable`, which is how `CachingRegistry` can modify it even though it asks for const-references of `ID`s.
	>
	> Additionally, considering KTech is designed to work single threaded, keeping a direct pointer to a world structure retrieved from `CachingRegistry` for the duration of a single function, after validating it (using `CachingRegistry::Exists()` or checking that the returned pointer is not `nullptr`), is valid practice, as nothing external should erase the pointed world structure from memory in the meantime.

	@see `KTech::ID`
*/
template<typename T>
class KTech::CachingRegistry
{
public:
	/*!
		@brief Retrieve structure using its `ID`.

		@param [in,out] id The structure's `ID`. Updates its cached index if it's stale.

		@return Pointer to the structure.
		@return `nullptr` if the structure was not found.
	*/
	auto operator[](const ID<T>& id) -> T*
	{
		if (!m_vec.empty())
		{
			for (size_t i = (id.m_i < m_vec.size() ? id.m_i : m_vec.size() - 1);; i--)
			{
				if (m_vec[i]->m_id == id)
				{
					id.m_i = i;
					return m_vec[i];
				}
				if (i == 0)
				{
					break;
				}
			}
		}
		id.m_i = 0;
		return nullptr;
	}

	/*!
		@brief Check if an `ID` matches a registered structure.

		@param [in,out] id The structure's `ID`. Updates its cached index if it's stale.

		@return `true`: the structure exists.
		@return `false`: ther structure doesn't exist.
	*/
	auto Exists(const ID<T>& id) -> bool
	{
		return IDToIndex(id) != m_vec.size();
	}

private:
	std::vector<T*> m_vec;

	// Adds the pointer to the container.
	// Automatically called by objects, layers, cameras and maps for themselves.
	// You shouldn't call this manually on a structure.
	auto Add(T* structure) -> ID<T>
	{
		structure->m_id.m_i = m_vec.size();
		m_vec.push_back(structure);
		return m_vec[m_vec.size() - 1]->m_id;
	}

	// Remove a structure from storage (doesn't delete it's memory).
	// Returns true if the structure was found and removed.
	// Returns false if the structure is missing.
	auto Remove(const ID<T>& id) -> bool
	{
		size_t toRemove = IDToIndex(id);
		if (toRemove == m_vec.size())
		{
			return false;
		}
		m_vec.erase(m_vec.begin() + toRemove);
		return true;
	}

	// Returns the valid index of the ID.
	// If the UUID is missing, return the size of the array making the index invalid.
	auto IDToIndex(const ID<T>& id) -> size_t
	{
		if (!m_vec.empty())
		{
			for (size_t i = (id.m_i < m_vec.size() ? id.m_i : m_vec.size() - 1);; i--)
			{
				if (m_vec[i]->m_id == id)
				{
					id.m_i = i;
					return i;
				}
				if (i == 0)
				{
					break;
				}
			}
		}
		id.m_i = 0;
		return m_vec.size();
	}

	friend T;
	friend class Memory;
};