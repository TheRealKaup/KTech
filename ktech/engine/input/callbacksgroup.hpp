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

#include "input.hpp"

struct KTech::Input::CallbacksGroup
{
	enum class Status : uint8_t
	{
		disabled,
		enabled,
		removeDisabled, // Remove and then return status to disabled
		removeEnabled, // Remove and then return status to enabled
	};

	std::vector<Callback*> m_callbacks;
	Status m_status;
	bool m_synced = true;

	CallbacksGroup(bool enabled = true)
		: m_status(enabled ? Status::enabled : Status::disabled) {}

	void AddCallback(Callback* callback);

	void DeleteCallbacks();

	void Enable();
	void Disable();

	void Update();
};