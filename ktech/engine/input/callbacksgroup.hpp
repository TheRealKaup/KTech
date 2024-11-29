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

class KTech::Input::CallbacksGroup
{
public:
	Engine& engine;

	CallbacksGroup(Engine& engine, bool enabled = true);
	~CallbacksGroup();

	void RegisterCallback(const std::string& stringKey, const std::function<bool()>& callback);
	void RegisterRangedCallback(char key1, char key2, const std::function<bool()>& callback);

	void Enable();
	void Disable();

private:
	enum class Status : uint8_t
	{
		disabled,
		enabled
	};

	std::vector<std::shared_ptr<Callback>> m_callbacks;
	Status m_status;
	bool m_synced = false;

	void Update();

	friend Input;
};