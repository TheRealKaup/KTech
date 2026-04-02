/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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

#include "callbackgroup.hpp"

#include "../../engine/engine.hpp"
#include "callback.hpp"

KTech::Input::CallbackGroup::CallbackGroup(Engine& p_engine, bool p_enabled)
	: engine(p_engine), m_enabled(p_enabled)
{
	p_engine.input.RegisterCallbackGroup(this);
}

KTech::Input::CallbackGroup::~CallbackGroup()
{
	// Set callbacks for deletion
	for (const std::shared_ptr<Callback>& callback : m_callbacks)
	{
		callback->status = Callback::Status::awaitingDeletion;
	}
	// Set this to be removed from memory
	engine.input.SetCallbackGroupToBeRemoved(this);
}

void KTech::Input::CallbackGroup::RegisterCallback(
	const std::string& p_stringKey, const std::function<bool()>& p_callback
)
{
	m_callbacks.push_back(engine.input.CreateCallback(p_stringKey, p_callback));
	m_synced = false;
}

void KTech::Input::CallbackGroup::RegisterRangedCallback(
	char p_start, char p_end, const std::function<bool()>& p_callback
)
{
	m_callbacks.push_back(engine.input.CrateRangedCallback(p_start, p_end, p_callback));
	m_synced = false;
}

void KTech::Input::CallbackGroup::Enable()
{
	m_enabled = true;
	m_synced = false;
}

void KTech::Input::CallbackGroup::Disable()
{
	m_enabled = false;
	m_synced = false;
}

void KTech::Input::CallbackGroup::Update()
{
	if (m_synced)
	{
		return;
	}
	if (m_enabled)
	{
		// Enable the callbacks
		for (const std::shared_ptr<Callback>& callback : m_callbacks)
		{
			callback->status = Callback::Status::enabled;
		}
	}
	else
	{
		// Disable the callbacks
		for (const std::shared_ptr<Callback>& callback : m_callbacks)
		{
			callback->status = Callback::Status::disabled;
		}
	}
	m_synced = true;
}
