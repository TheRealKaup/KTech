/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 Ethan Kaufman (AKA Kaup)

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

#include "ktech.hpp"

#define maxInputLength 7

KTech::IO::BasicHandler::BasicCallback* KTech::IO::RegisterCallback(const std::string& input, const std::function<void()>& callback, bool onTick)
{
	if (callback == nullptr) // Avoid constantly checking later whether callback is null
		return nullptr;
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < basicHandlers.size(); i++)
	{
		if (basicHandlers[i]->input == input)
		{
			basicHandlers[i]->callbacks.push_back(new BasicHandler::BasicCallback(callback, basicHandlers[i], onTick));
			return basicHandlers[i]->callbacks[basicHandlers[i]->callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	basicHandlers.push_back(new BasicHandler(input));
	// And add a callback to it
	basicHandlers[i]->callbacks.push_back(new BasicHandler::BasicCallback(callback, basicHandlers[i], onTick));
	return basicHandlers[i]->callbacks[basicHandlers[i]->callbacks.size() - 1]; // Last callback of last handler
}

KTech::IO::RangedHandler::RangedCallback* KTech::IO::RegisterRangedCallback(char key1, char key2, const std::function<void()>& callback)
{
	if (callback == nullptr) // Avoid constantly checking later whether callback is null
		return nullptr;
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < rangedHandlers.size(); i++)
	{
		if (rangedHandlers[i]->key1 == key1 && rangedHandlers[i]->key2 == key2)
		{
			rangedHandlers[i]->callbacks.push_back(new RangedHandler::RangedCallback(callback, rangedHandlers[i]));
			return rangedHandlers[i]->callbacks[rangedHandlers[i]->callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	rangedHandlers.push_back(new RangedHandler(key1, key2));
	// And add a callback to it
	rangedHandlers[i]->callbacks.push_back(new RangedHandler::RangedCallback(callback, rangedHandlers[i]));
	return rangedHandlers[i]->callbacks[rangedHandlers[i]->callbacks.size() - 1]; // Last callback
}

void KTech::IO::Call()
{
	Log("<IO::Call()> Start of function...", RGBColors::hotPink);
	// Update groups' callbacks' `enabled` if `synced` is false
	Log("<IO::Call()> Searching for unsynced groups", RGBColors::hotPink);
	for (CallbacksGroup*& group : groups)
	{
		if (!group->synced)
		{
			Log("<IO::Call()> Found an unsynced group", RGBColors::hotPink);
			switch (group->status)
			{
				case CallbacksGroup::Status::disabled:
				{
					Log("<IO::Call()> Disabling group", RGBColors::hotPink);
					// Disalbe the callbacks
					for (BasicHandler::BasicCallback* basicCallback : group->basicCallbacks)
						basicCallback->enabled = false;
					for (RangedHandler::RangedCallback* rangedCallback : group->rangedCallbacks)
						rangedCallback->enabled = false;
					break;
				}
				case CallbacksGroup::Status::enabled:
				{
					Log("<IO::Call()> Enabling group", RGBColors::hotPink);
					// Enable the callbacks
					for (BasicHandler::BasicCallback* basicCallback : group->basicCallbacks)
						basicCallback->enabled = true;
					for (RangedHandler::RangedCallback* rangedCallback : group->rangedCallbacks)
						rangedCallback->enabled = true;
					break;
				}
				case CallbacksGroup::Status::removeDisabled:
				{
					Log("<IO::Call()> Deleting group's callbacks then disabling", RGBColors::hotPink);
					// Delete the callbacks from memory (which will automatically delete the callbacks from
					// their parent handlers' callback vector)
					for (BasicHandler::BasicCallback*& basicCallback : group->basicCallbacks)
					{
						Log("<IO::Call()> Deleting basic callback " + std::to_string((size_t)basicCallback), RGBColors::hotPink);
						Log("<IO::Call()> Callback's parent handler is " + std::to_string((size_t)(basicCallback->parentHandler)), RGBColors::hotPink);
						delete basicCallback;
					}
					for (RangedHandler::RangedCallback*& rangedCallback : group->rangedCallbacks)
						delete rangedCallback;
					// Clearthe group's vectors
					group->basicCallbacks.clear();
					group->rangedCallbacks.clear();
					// Disable the group as requested
					group->status = CallbacksGroup::Status::disabled;
					break;
				}
				case CallbacksGroup::Status::removeEnabled:
				{
					Log("<IO::Call()> Deleting group's callbacks then enabling", RGBColors::hotPink);
					// The same as ^ but enable the group afterwards
					for (BasicHandler::BasicCallback* basicCallback : group->basicCallbacks)
					{
						Log("<IO::Call()> Deleting basic callback " + std::to_string((size_t)basicCallback), RGBColors::hotPink);
						Log("<IO::Call()> Callback's parent handler is " + std::to_string((size_t)(basicCallback->parentHandler)), RGBColors::hotPink);
						delete basicCallback;
					}
					for (RangedHandler::RangedCallback* rangedCallback : group->rangedCallbacks)
						delete rangedCallback;
					group->basicCallbacks.clear();
					group->rangedCallbacks.clear();
					group->status = CallbacksGroup::Status::enabled;
					break;
				}
			}
			group->synced = true;
		}
	}
	// Call basic handlers' callbacks
	Log("<IO::Call()> Searching for onTick basic callbacks", RGBColors::hotPink);
	for (BasicHandler*& handler : basicHandlers)
	{
		if (handler->timesPressed > 0)
		{
			for (BasicHandler::BasicCallback*& callback : handler->callbacks)
			{
				if (callback->enabled && callback->onTick)
				{
					input = handler->input; // Would be preferable to not copy it this way each iteration
					Log("<IO::Call()> Calling found onTick basic callback #" + std::to_string((size_t)callback), RGBColors::hotPink);
					Log("<IO::Call()> Callback's parent handler is " + std::to_string((size_t)(handler)), RGBColors::hotPink);
					callback->ptr();
				}
			}
		}
	}
	Log("<IO::Call()> End of function.", RGBColors::hotPink);
}

char* KTech::IO::Get()
{
	Log("<IO::Get()> Start of function...", RGBColors::hotPink);
	static char* buf = new char[maxInputLength];
	
	// Clear previous buffer
	Log("<IO::Get()> Clearing previous buffer", RGBColors::hotPink);
	memset(buf, 0, maxInputLength);
	// Read input to buffer (blocking)
	Log("<IO::Get()> Reading to buffer (blocking)", RGBColors::hotPink);
	read(0, buf, maxInputLength);

	Log("<IO::Get()> Returning buffer", RGBColors::hotPink);
	return buf;
}

void KTech::IO::Loop()
{
	Log("<IO::Loop()> Start of function...", RGBColors::hotPink);
	while (engine->running)
	{
		Log("<IO::Loop()> Start of iteratin, getting input", RGBColors::hotPink);
		// Get input and update `std::string Input::input`
		input.assign(Get());
		// Quit
		if (input == quitKey)
		{
			Log("<IO::Loop()> Quit key detected, exiting loop", RGBColors::hotPink);
			engine->running = false;
			break;
		}
		// Call basic handler
		Log("<IO::Loop()> Searching for a basic handler", RGBColors::hotPink);
		for (size_t i = 0; i < basicHandlers.size(); i++)
		{
			if (input == basicHandlers[i]->input) // The input is of this handler, call handler's callbacks
			{
				Log("<IO::Loop()> Found the basic handler to call", RGBColors::hotPink);
				basicHandlers[i]->timesPressed++; // Used in synced calling
				for (size_t j = 0; j < basicHandlers[i]->callbacks.size(); j++) // Call the calls
				{
					if (basicHandlers[i]->callbacks[j]->enabled && !basicHandlers[i]->callbacks[j]->onTick) // Check if the callback shouldn't be called on tick
					{
						Log("<IO::Loop()> Calling basic handler's callback #" + std::to_string(i), RGBColors::hotPink);
						basicHandlers[i]->callbacks[j]->ptr(); // Call
					}				
				}
				break; // Break because there shouldn't be a similar handler
			}
		}
		// Call ranged handler, only if the input is a single character
		if (input.length() == 1)
		{
			Log("<IO::Loop()> Searching for a ranged handler", RGBColors::hotPink);
			for (size_t i = 0; i < rangedHandlers.size(); i++)
			{
				if (rangedHandlers[i]->key1 <= input[0] && input[0] <= rangedHandlers[i]->key2) // If the key is in range
				{
					Log("<IO::Loop()> Found ranged handler", RGBColors::hotPink);
					for (RangedHandler::RangedCallback*& callback: rangedHandlers[i]->callbacks)
					{
						if (callback->enabled)
						{
							Log("<IO::Loop()> Calling ranged handler's callback #" + std::to_string(i), RGBColors::hotPink);	
							callback->ptr(); // Call
						}
					}
					// Don't break since there can be more handlers which manage an area in which the input is located in
				}
			}
		}
	}
	Log("<IO::Loop()> End of function.", RGBColors::hotPink);
}

bool KTech::IO::Is(const std::string& stringKey)
{
	return (input == stringKey);
}

bool KTech::IO::Is(char charKey)
{
	return input.length() == 1 && input[0] == charKey;
}

uint8_t KTech::IO::GetInt()
{
	return input[0] - '0';
}

bool KTech::IO::Bigger(char charKey)
{
	return (input[0] >= charKey) && (input[1] == 0);
}

bool KTech::IO::Smaller(char charKey)
{
	return (input[0] <= charKey) && (input[1] == 0);
}

bool KTech::IO::Between(char charKey1, char charKey2)
{
	return (input[0] >= charKey1) && (input[0] <= charKey2) && (input[1] == 0);
}

KTech::IO::CallbacksGroup* KTech::IO::CreateCallbacksGroup(bool enabled)
{
	Log("<IO::CreateCallbacksGroup()> Creating and pushing new CallbacksGroup.", RGBColors::hotPink);
	groups.push_back(new CallbacksGroup(enabled));
	return groups[groups.size() - 1];
}

KTech::IO::BasicHandler::BasicCallback::~BasicCallback()
{
	Log("<IO::BasicCallback::~BasicCallback()> Requesting parent handler to remove this callback.", RGBColors::hotPink);
	parentHandler->RemoveCallback(this);
}

KTech::IO::RangedHandler::RangedCallback::~RangedCallback()
{
	Log("<IO::RangedCallback::~RangedCallback()> Requesting parent handler to remove this callback.", RGBColors::hotPink);
	parentHandler->RemoveCallback(this);
}

void KTech::IO::BasicHandler::RemoveCallback(BasicCallback* callback)
{
	for (size_t i = 0; i < callbacks.size(); i++)
	{
		if (callbacks[i] == callback)
		{
			Log("<IO::BasicCallback::~BasicCallback()> Clearing found pointer", RGBColors::hotPink);
			callbacks.erase(callbacks.begin() + i);
		}
	}
}

void KTech::IO::RangedHandler::RemoveCallback(RangedCallback* callback)
{
	for (size_t i = 0; i < callbacks.size(); i++)
	{
		if (callbacks[i] == callback)
		{
			Log("<IO::RangedCallback::~RangedCallback()> Clearing found pointer", RGBColors::hotPink);
			callbacks.erase(callbacks.begin() + i);
		}
	}
}

void KTech::IO::CallbacksGroup::DeleteCallbacks()
{
	// Disable as soon as possible the callbacks (since the actual callbacks are probably deleted right after)
	Log("<IO::CallbacksGroup::DeleteCallbacks()> Start of function...", RGBColors::hotPink);
	for (BasicHandler::BasicCallback* basicCallback : basicCallbacks)
		basicCallback->enabled = false;
	for (RangedHandler::RangedCallback* rangedCallback : rangedCallbacks)
		rangedCallback->enabled = false;
	// Then request to delete the callbacks
	// (removeDisabled will set the group to be disabled later, removeEnabled will set to enabled)
	status = (status == Status::disabled ? Status::removeDisabled : Status::removeEnabled);
	synced = false;
	Log("<IO::CallbacksGroup::DeleteCallbacks()> Set callbacks to be deleted in the next iteration of `IO::Call()`", RGBColors::hotPink);
	Log("<IO::CallbacksGroup::DeleteCallbacks()> End of function.", RGBColors::hotPink);
}