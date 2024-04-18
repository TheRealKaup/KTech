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

#include "callbacks_handlers.hpp"

#include "../../utility/rgbcolors.hpp"
#include "callbacksgroup.hpp"

KTech::Input::BasicCallback::~BasicCallback()
{
	TempLog("<Input::BasicCallback::~BasicCallback()> Requesting parent handler to remove this callback.", RGBColors::hotPink);
	parentHandler->RemoveCallback(this);
}

KTech::Input::RangedCallback::~RangedCallback()
{
	TempLog("<Input::RangedCallback::~RangedCallback()> Requesting parent handler to remove this callback.", RGBColors::hotPink);
	parentHandler->RemoveCallback(this);
}

void KTech::Input::BasicHandler::RemoveCallback(BasicCallback* p_callback)
{
	for (size_t i = 0; i < m_callbacks.size(); i++)
	{
		if (m_callbacks[i] == p_callback)
		{
			TempLog("<Input::BasicCallback::~BasicCallback()> Clearing found pointer", RGBColors::hotPink);
			m_callbacks.erase(m_callbacks.begin() + i);
		}
	}
}

void KTech::Input::RangedHandler::RemoveCallback(RangedCallback* p_callback)
{
	for (size_t i = 0; i < m_callbacks.size(); i++)
	{
		if (m_callbacks[i] == p_callback)
		{
			TempLog("<Input::RangedCallback::~RangedCallback()> Clearing found pointer", RGBColors::hotPink);
			m_callbacks.erase(m_callbacks.begin() + i);
		}
	}
}