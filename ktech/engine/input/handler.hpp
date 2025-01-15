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

#include <memory>

#include "input.hpp"

struct KTech::Input::Handler
{
	const char m_start = '\0', m_end = '\0';
	const std::string m_string;
	std::vector<std::shared_ptr<Callback>> m_callbacks;

	Handler(std::string input)
		: m_string(std::move(input)) {}

	Handler(char key1, char key2)
		: m_start(key1), m_end(key2) {}

	void RemoveCallbacksSetToBeDeleted();
};