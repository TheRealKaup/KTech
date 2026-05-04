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
template <typename T>
class KTech::CachingRegistry
{
private:
	/*!
		@brief Pointers of the registered entities.

		Note that it is usually not safe to iterate over this vector using a ranged-based for loop, because entities may be added as a result of calling OnTick of other functions (and if enough entities are added, the vector is reallocated to increase the capacity). Hence, `Memory::CallOnTicks()` is ought to iterate over this vector using an index.

		Entities are removed from a `CachingRegistry` in 2 steps, first by setting the pointers to null (`CachingRegistry::Remove()`), and then by removing all null pointers (`CachingRegistry::Prune()`). This is done to prevent iterator invalidation in `Memory::CallOnTicks()`.
	*/
	std::vector<T*> m_vec;

	/*!
		@brief Retrieve structure using its `ID`.

		@param [in,out] id The structure's `ID`. Updates its cached index if it's stale.

		@return Pointer to the structure.
		@return `nullptr` if the structure was not found.
	*/
	auto operator[](const KTech::ID<T>& id) -> T*;

	// Adds the pointer to the container.
	// Automatically called by objects, layers, cameras and maps for themselves.
	// You shouldn't call this manually on a structure.
	void Add(T* structure);

	/*!
		@brief Causes the removal of an entity by replacing its pointer in `CachingRegistry::m_vec` with `nullptr`.

		@param id ID of the entity.
	*/
	void Remove(const KTech::ID<T>& id);

	/*!
		@brief Update the cached index of an `ID`.

		Can be used to check if an entity exists in memory.

		@param id `ID` to update, which may have a stale index.
		@return `true` on success, `false` on failure (because ID was not found, e.g., was removed).
	*/
	auto UpdateID(const KTech::ID<T>& id) -> bool;

	/*!
		@brief Erase `nullptr` elements from `CachingRegistry::m_vec`.
	*/
	void Prune();

	friend class Memory;
};
