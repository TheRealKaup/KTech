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

/*!
	@brief Serializable world structure identifier.

	Comprises a universally unique identifier (UUID) and a local cached index maintained by `CachingRegistry`. The local cached index is `mutable`, so it can be changed by `CachingRegistry` even if the containing `ID` is `const`.

	@see `KTech::CachingRegistry`
*/
template<typename T>
struct KTech::ID
{
	//! @brief Construct a null `ID`.
	explicit constexpr ID()
		: m_uuid(0) {};

	/*!
		@brief Compare 2 `ID`s.
		@param [in] id The `ID` to compare with this `ID`.
		@return `true`: the UUIDs are equal (the cached index is ignored). `false`: they are unequal.
	*/
	constexpr auto operator==(ID id) const -> bool
	{
		return id.m_uuid == m_uuid;
	}

private:
	mutable size_t m_i = 0; // Local cached index
	uint64_t m_uuid; // Universally unique identifier

	explicit constexpr ID(uint64_t uuid)
		: m_uuid(uuid) {}

	// Generate a unique `ID`; can only be used by world structures.
	static auto Unique() -> ID
	{
		static uint64_t uuid = 0;
		uuid++;
		return ID(uuid);
	}

	friend T; // Allow only world structures to call `Unique()`.
	friend class CachingRegistry<T>;
};