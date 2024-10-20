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

struct KTech::Input::Callback
{
	bool enabled = true;
	const std::function<bool()> ptr;
	Handler* const parentHandler;

	inline Callback(const std::function<bool()>& callback, Handler* parentHandler)
		: ptr(callback), parentHandler(parentHandler) {}
	
	~Callback();
};

struct KTech::Input::Handler
{
	enum class Type : uint8_t
	{
		String,
		Range
	};

	const Type m_type;
	const char m_key1 = '\0', m_key2 = '\0';
	const std::string m_input;
	std::vector<Callback*> m_callbacks;

	inline Handler(const::std::string& input)
		: m_input(input), m_type(Type::String) {}

	inline Handler(char key1, char key2)
		: m_key1(key1), m_key2(key2), m_type(Type::Range) {}

	void RemoveCallback(Callback* callback);
};