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

struct KTech::Input::BasicCallback
{
	bool enabled = true;
	std::function<bool()> ptr;
	BasicHandler* parentHandler;
	bool onTick;
	
	inline BasicCallback(const std::function<bool()>& callback, BasicHandler* parentHandler, bool onTick)
		: ptr(callback), parentHandler(parentHandler), onTick(onTick) {}
	
	~BasicCallback();
};

struct KTech::Input::RangedCallback
{
	bool enabled = true;
	std::function<bool()> ptr;
	RangedHandler* parentHandler;

	inline RangedCallback(const std::function<bool()>& callback, RangedHandler* parentHandler)
		: ptr(callback), parentHandler(parentHandler) {}
	
	~RangedCallback();
};

struct KTech::Input::BasicHandler
{
	const std::string m_input;
	std::vector<BasicCallback*> m_callbacks; // Callbacks are stored as pointers because the vector changes its size, and CallbackGroups need a consistent pointer to the their callbacks.
	uint8_t m_timesPressed = 0;
	
	inline BasicHandler(const std::string& input)
		: m_input(input) {};
	
	void RemoveCallback(BasicCallback*);
};

struct KTech::Input::RangedHandler
{
	const char m_key1, m_key2;
	std::vector<RangedCallback*> m_callbacks; // Callbacks are stored as pointers because the vector changes its size, and CallbackGroups need a consistent pointer to the their callbacks.
	
	inline RangedHandler(char key1, char key2)
		: m_key1(key1), m_key2(key2) {};

	void RemoveCallback(RangedCallback*);
};