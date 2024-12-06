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

#include "callback.hpp"
#include "../../engine/engine.hpp"

/*!
	Register a new `CallbacksGroup` at `Input`.

	@param [in] engine Parent engine.
	@param [in] enabled Will set registered callbacks to this value by default.
*/
KTech::Input::CallbacksGroup::CallbacksGroup(Engine& engine, bool enabled)
	: engine(engine), m_status(enabled ? Status::enabled : Status::disabled)
{
	engine.input.RegisterCallbacksGroup(this);
}

/*!
	Remove `CallbacksGroup` and its callback functions from `Input`.

	Deconstructing a `CallbacksGroup` also means all of its input callback functions will be permanently deregistered. This is good, because you can create a `CallbacksGroup` instance in your class (like a moving character inherited from `Object`), register member functions as input callbacks, and not worry later about your character deconstructing while still having member functions registered and potentially called (because the `CallbacksGroup` and its callback functions are deconstructed with it).
*/
KTech::Input::CallbacksGroup::~CallbacksGroup()
{
	// Set callbacks for deletion
	for (const std::shared_ptr<Callback>& callback : m_callbacks)
	{
		callback->status = Callback::Status::awaitingDeletion;
	}
	// Set this to be removed from memory
	engine.input.SetCallbacksGroupToBeRemoved(this);
}

/*!
	@fn CallbacksGroup::RegisterCallback
	@brief Register a function to be called back when an input is received.

	Input is received in escape sequences. `KTech::Keys` includes such values that you can give to the `stringKey` parameter.

	If your function is a callback for different inputs, you can use `Input::input` from within your function to find out which input called it.

	If the callbacks group is currently enabled, the new callback will be enabled as well only in the next tick. This behavior is explained in `CallbacksGroup::Enable()`'s documentation entry.

	@param [in] stringKey Escape sequence that when received calls your `callback` function.
	@param [in] callback Your callback function which returns `bool`. The return value is explained in `Output::ShouldRenderThisTick()`.

	@see `KTech::Keys`
	@see `Output::ShouldRenderThisTick()`
*/
void KTech::Input::CallbacksGroup::RegisterCallback(const std::string& p_stringKey, const std::function<bool()>& p_callback)
{
	m_callbacks.push_back(engine.input.CreateCallback(p_stringKey, p_callback));
	m_synced = false;
}

/*!
	@fn CallbacksGroup::RegisterRangedCallback
	@brief Register a function to be called back when an input within a characters range is received.

	Any input received within the range will call your callback function. Because it's a range, the inputs are limited in length to 1 character. Anyway, this function is mostly useful when 1-character-long inputs are needed. For example:

	@code{.cpp}
	bool LogLetters()
	{
		Output::Log(Input::input, RGBColors::green);
		return true;
	}

	//...
		myCallbacksGroup.RegisterRangedCallback('a', 'z', LogLetters); // Notice that the inputs `a`-`z` don't include `A`-`sZ`
		myCallbacksGroup.RegisterRangedCallback('A', 'Z', LogLetters);
	//...
	@endcode

	You can use `Input::input` from within your callback function to find out which input called it.

	If the callbacks group is currently enabled, the new callback will be enabled as well only in the next tick. This behavior is explained in `CallbacksGroup::Enable()`'s documentation entry.

	@param [in] start Start of (ASCII) characters range.
	@param [in] end End of (ASCII) characters range.
	@param [in] callback Your callback function which returns `bool`. The return value is explained in `Output::ShouldRenderThisTick()`.

	@see `CallbacksGroup::Enable()`
	@see `Output::ShouldRenderThisTick()`
*/
void KTech::Input::CallbacksGroup::RegisterRangedCallback(char p_start, char p_end, const std::function<bool()>& p_callback)
{
	m_callbacks.push_back(engine.input.CrateRangedCallback(p_start, p_end, p_callback));
	m_synced = false;
}

/*!
	Enable registered callback functions.

	The callbacks will be enabled only in next tick. This way, for example, you don't have to worry about a sequence where:

	1. button1 shows and selects button2 (enableing button2's `CallbacksGroup`).
	2. button2 being  immediately pressed as well (because both buttons are pressed using the same key).

	Which is clearly not the wanted result. The desired behavior is to let the player see the next button, and have to press twice if they wish to press on both buttons.

	This behavior of "update only in the next tick" applies to `CallbacksGroup::Disable()`, `CallbacksGroup::RegisterCallback()` and `CallbacksGroup::RegisterRangedCallback()` as well.
*/
void KTech::Input::CallbacksGroup::Enable()
{
	m_status = Status::enabled;
	m_synced = false;
}

/*!
	Disable registered callback functions.

	The callbacks will be disabled only in the next tick. This behavior is explained in `CallbacksGroup::Enable()`'s documentation entry.

	@see `CallbacksGroup::Enable()`
*/
void KTech::Input::CallbacksGroup::Disable()
{
	m_status = Status::disabled;
	m_synced = false;
}

void KTech::Input::CallbacksGroup::Update()
{
	if (m_synced)
	{
		return;
	}
	switch (m_status)
	{
		case Status::disabled:
		{
			// Disable the callbacks
			for (const std::shared_ptr<Callback>& callback : m_callbacks)
			{
				callback->status = Callback::Status::disabled;
			}
			break;
		}
		case Status::enabled:
		{
			// Enable the callbacks
			for (const std::shared_ptr<Callback>& callback : m_callbacks)
			{
				callback->status = Callback::Status::enabled;
			}
			break;
		}
	}
	m_synced = true;
}