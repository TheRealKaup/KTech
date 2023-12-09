/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 E. Kaufman (AKA Kaup)

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
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < basicHandlers.size(); i++)
	{
		if (basicHandlers[i].input == input)
		{
			basicHandlers[i].callbacks.push_back(new BasicHandler::BasicCallback(callback, &(basicHandlers[i]), onTick));
			return basicHandlers[i].callbacks[basicHandlers[i].callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	basicHandlers.push_back(BasicHandler(input));
	// And add a callback to it
	basicHandlers[i].callbacks.push_back(new BasicHandler::BasicCallback(callback, &basicHandlers[i], onTick));
	return basicHandlers[i].callbacks[basicHandlers[i].callbacks.size() - 1]; // Last callback of last handler
}

KTech::IO::RangedHandler::RangedCallback* KTech::IO::RegisterRangedCallback(char key1, char key2, const std::function<void()>& callback)
{
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < rangedHandlers.size(); i++)
	{
		if (rangedHandlers[i].key1 == key1 && rangedHandlers[i].key2 == key2)
		{
			rangedHandlers[i].callbacks.push_back(new RangedHandler::RangedCallback(callback, &rangedHandlers[i]));
			return rangedHandlers[i].callbacks[rangedHandlers[i].callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	rangedHandlers.push_back(RangedHandler(key1, key2));
	// And add a callback to it
	rangedHandlers[i].callbacks.push_back(new RangedHandler::RangedCallback(callback, &rangedHandlers[i]));
	return rangedHandlers[i].callbacks[rangedHandlers[i].callbacks.size() - 1]; // Last callback
}

void KTech::IO::Call()
{
	// Update groups' callbacks' `enabled` if `synced` is false
	for (CallbacksGroup& group : groups)
	{
		if (!group.synced)
		{
			for (BasicHandler::BasicCallback* basicCallback : group.basicCallbacks)
				basicCallback->enabled = group.enabled;
			for (RangedHandler::RangedCallback* rangedCallback : group.rangedCallbacks)
				rangedCallback->enabled = group.enabled;
			group.synced = true;
		}
	}
	// Call basic handlers' callbacks
	for (BasicHandler& handler : basicHandlers)
	{
		if (handler.timesPressed > 0)
		{
			for (BasicHandler::BasicCallback*& callback : handler.callbacks)
			{
				if (callback->enabled && callback->onTick && callback->callback)
				{
					input = handler.input;
					callback->callback();
				}
			}
			handler.timesPressed = 0;
		}
	}
}

char* KTech::IO::Get()
{
	static char* buf = new char[maxInputLength];
	
	// Clear previous buffer
	memset(buf, 0, maxInputLength);
	// Read to buffer (blocking)
	read(0, buf, maxInputLength);

	return buf;
}

void KTech::IO::Loop()
{
	while (engine->running)
	{
		// Get input and update `std::string Input::input`
		input.assign(Get());
		// Quit
		if (input == quitKey)
		{
			engine->running = false;
			break;
		}
		// Call basic handlers
		for (size_t i = 0; i < basicHandlers.size(); i++)
		{
			if (input == basicHandlers[i].input) // If the strings are equal
			{
				basicHandlers[i].timesPressed++;
				for (size_t j = 0; j < basicHandlers[i].callbacks.size(); j++) // Call the calls
					if (basicHandlers[i].callbacks[j]->enabled && !basicHandlers[i].callbacks[j]->onTick) // Check if the callback is enabled and shouldn't be called on tick
						basicHandlers[i].callbacks[j]->callback(); // Call
				break; // Break because there shouldn't be a similar handler
			}
		}
		// Call ranged handlers, only if the input is 1 letter in length
		if (input.length() == 1)
		{
			for (size_t i = 0; i < rangedHandlers.size(); i++)
			{
				if (rangedHandlers[i].key1 <= input[0] && input[0] <= rangedHandlers[i].key2) // If the key is in range
				{
					for (RangedHandler::RangedCallback*& callback: rangedHandlers[i].callbacks)
						if (callback->enabled) // Check if the callback is enabled
							callback->callback(); // Call
					// Don't break since there can be more handlers which manage an area in which the input is located in
				}
			}
		}
	}
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

size_t KTech::IO::CreateCallbackGroup(bool enabled)
{
	groups.push_back(CallbacksGroup(enabled));
	return groups.size() - 1;
}

void KTech::IO::CallbacksGroup::DeleteCallbacks()
{
	// Delete memory (also calls the callbacks' destructors which removes themselves from their parent handlers)
	for (BasicHandler::BasicCallback* basicCallback : basicCallbacks)
		delete basicCallback;
	for (RangedHandler::RangedCallback* rangedCallback : rangedCallbacks)
		delete rangedCallback;
	// Clear this group's vector.
	basicCallbacks.clear();
	rangedCallbacks.clear();
}

KTech::IO::BasicHandler::BasicCallback::~BasicCallback()
{
	Log("<BasicCallback::~BasicCallback()> Start of function...", RGB(255, 0, 255));
	for (size_t i = 0; i < parentHandler->callbacks.size(); i++)
		if (this == parentHandler->callbacks[i])
			parentHandler->callbacks.erase(parentHandler->callbacks.begin() + i);
	Log("<BasicCallback::~BasicCallback()> End of function.", RGB(255, 0, 255));
}

KTech::IO::RangedHandler::RangedCallback::~RangedCallback()
{
	Log("<RangedCallback::~RangedCallback()> Start of function...", RGB(255, 0, 255));
	for (size_t i = 0; i < parentHandler->callbacks.size(); i++)
		if (this == parentHandler->callbacks[i])
			parentHandler->callbacks.erase(parentHandler->callbacks.begin() + i);
	Log("<RangedCallback::~RangedCallback()> End of function.", RGB(255, 0, 255));
}