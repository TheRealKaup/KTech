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

#include "callbacksgroup.hpp"

#include "../../utility/rgbcolors.hpp"
#include "callbacks_handlers.hpp"

void KTech::Input::CallbacksGroup::AddCallback(BasicCallback* callback)
{
	m_basicCallbacks.push_back(callback);
	callback->enabled = false;
}

void KTech::Input::CallbacksGroup::AddCallback(RangedCallback* callback)
{
	m_rangedCallbacks.push_back(callback);
	callback->enabled = false;
}

void KTech::Input::CallbacksGroup::Enable()
{
	m_status = Status::enabled;
	m_synced = false;
}

void KTech::Input::CallbacksGroup::Disable()
{
	m_status = Status::disabled;
	m_synced = false;
}

void KTech::Input::CallbacksGroup::DeleteCallbacks()
{
	// Disable as soon as possible the callbacks (since the actual callbacks are probably deleted right after)
	TempLog("<Input::CallbacksGroup::DeleteCallbacks()> Start of function...", RGBColors::hotPink);
	for (BasicCallback* basicCallback : m_basicCallbacks)
		basicCallback->enabled = false;
	for (RangedCallback* rangedCallback : m_rangedCallbacks)
		rangedCallback->enabled = false;
	// Then request to delete the callbacks
	// (removeDisabled will set the group to be disabled later, removeEnabled will set to enabled)
	m_status = (m_status == Status::disabled ? Status::removeDisabled : Status::removeEnabled);
	m_synced = false;
	TempLog("<Input::CallbacksGroup::DeleteCallbacks()> Set callbacks to be deleted in the next iteration of `Input::Call()`", RGBColors::hotPink);
	TempLog("<Input::CallbacksGroup::DeleteCallbacks()> End of function.", RGBColors::hotPink);
}

void KTech::Input::CallbacksGroup::Update()
{
	if (!m_synced)
	{
		TempLog("<Input::Call()> Found an unsynced group", RGBColors::hotPink);
		switch (m_status)
		{
			case Status::disabled:
			{
				TempLog("<Input::Call()> Disabling group", RGBColors::hotPink);
				// Disalbe the callbacks
				for (BasicCallback* basicCallback : m_basicCallbacks)
					basicCallback->enabled = false;
				for (RangedCallback* rangedCallback : m_rangedCallbacks)
					rangedCallback->enabled = false;
				break;
			}
			case Status::enabled:
			{
				TempLog("<Input::Call()> Enabling group", RGBColors::hotPink);
				// Enable the callbacks
				for (BasicCallback* basicCallback : m_basicCallbacks)
					basicCallback->enabled = true;
				for (RangedCallback* rangedCallback : m_rangedCallbacks)
					rangedCallback->enabled = true;
				break;
			}
			case Status::removeDisabled:
			{
				TempLog("<Input::Call()> Deleting group's callbacks then disabling", RGBColors::hotPink);
				// Delete the callbacks from memory (which will automatically delete the callbacks from
				// their parent handlers' callback vector)
				for (BasicCallback*& basicCallback : m_basicCallbacks)
				{
					TempLog("<Input::Call()> Deleting basic callback " + std::to_string((size_t)basicCallback), RGBColors::hotPink);
					TempLog("<Input::Call()> Callback's parent handler is " + std::to_string((size_t)(basicCallback->parentHandler)), RGBColors::hotPink);
					delete basicCallback;
				}
				for (RangedCallback*& rangedCallback : m_rangedCallbacks)
					delete rangedCallback;
				// Clearthe group's vectors
				m_basicCallbacks.clear();
				m_rangedCallbacks.clear();
				// Disable the group as requested
				m_status = CallbacksGroup::Status::disabled;
				break;
			}
			case Status::removeEnabled:
			{
				TempLog("<Input::Call()> Deleting group's callbacks then enabling", RGBColors::hotPink);
				// The same as ^ but enable the group afterwards
				for (BasicCallback* basicCallback : m_basicCallbacks)
				{
					TempLog("<Input::Call()> Deleting basic callback " + std::to_string((size_t)basicCallback), RGBColors::hotPink);
					TempLog("<Input::Call()> Callback's parent handler is " + std::to_string((size_t)(basicCallback->parentHandler)), RGBColors::hotPink);
					delete basicCallback;
				}
				for (RangedCallback* rangedCallback : m_rangedCallbacks)
					delete rangedCallback;
				m_basicCallbacks.clear();
				m_rangedCallbacks.clear();
				m_status = Status::enabled;
				break;
			}
		}
		m_synced = true;
	}
}