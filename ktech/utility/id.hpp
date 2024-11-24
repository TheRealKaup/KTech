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

template<class T>
struct KTech::ID
{
	std::size_t m_i; // Index

	inline ID()
		: m_i(0), m_uuid(GenerateUUID()) {}

	constexpr inline ID(size_t index, size_t uuid)
		: m_i(index), m_uuid(uuid) {}

	inline auto operator==(ID other) const -> bool
	{
		return other.m_uuid == m_uuid;
	}

	inline static auto GenerateUUID() -> uint64_t
	{
		static uint64_t uuid = 0;
		uuid++;
		return uuid;
	}

	[[nodiscard]] inline auto GetUUID() const -> uint64_t
	{
		return m_uuid;
	}

private:
	uint64_t m_uuid; // UUID
};