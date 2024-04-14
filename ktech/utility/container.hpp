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
	T** arr;
	// The size of the array
	size_t size = 0;

	// Returns the valid index of the ID.
	// If the UUID is missing, return the size of the array making the index invalid.
	// Fixes the ID if outdated.
	inline size_t IDToIndex(ID<T>& id);
	// Returns the valid index of the ID.
	// If the UUID is missing, return the size of the array making the index invalid.
	inline size_t IDToIndex(const ID<T>& id);

	// Returns true if the structure is found, false if missing.
	// Fixes the ID if outdated.
	bool Exists(ID<T>& id);
	// Returns true if the structure is found, false if missing.
	bool Exists(const ID<T>& id);

	// Adds the pointer to the container.
	// Automatically called by objects, layers, cameras and maps for themselves.
	// You shouldn't call this manualy on a structure.
	ID<T> Add(T* structure);

	// Remove a structure from storage (doesn't delete it's memory).
	// Returns true if the structure was found and removed.
	// Returns false if the structre is missing.
	bool Remove(const ID<T>& id);

	// Takes an ID reference, returns the pointer to the structure.
	// If the ID is outdated (made so by structures being removed) then update it.
	// If the ID's UUID doesn't exist in the storage at all, this will reset the ID and will return `nullptr`.
	T* operator[](ID<T>& id);
	// Takes an ID reference, returns the pointer to the structure.
	// Constant version of the previous operator, which will not update the given ID if it's outdated/missing.
	T* operator[](const ID<T>& id);
};