#include "engine.hpp"

#define maxInputLength 7

size_t Engine::Input::handlerIndex = -1;

std::string Engine::Input::input(maxInputLength, '\0');

// callbacks.push_back(new Handler::Callback(callback, onTick, this));

Engine::Input::BasicHandler::BasicCallback* Engine::Input::RegisterCallback(const std::string& input, std::function<void()> callback, bool onTick)
{
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < BasicHandler::handlers.size(); i++)
	{
		if (BasicHandler::handlers[i]->input == input)
		{
			BasicHandler::handlers[i]->callbacks.push_back(new BasicHandler::BasicCallback(callback, BasicHandler::handlers[i], onTick));
			return BasicHandler::handlers[i]->callbacks[BasicHandler::handlers[i]->callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	BasicHandler::handlers.push_back(new BasicHandler(input));
	// And add a callback to it
	BasicHandler::handlers[i]->callbacks.push_back(new BasicHandler::BasicCallback(callback, BasicHandler::handlers[i], onTick));
	return BasicHandler::handlers[i]->callbacks[BasicHandler::handlers[i]->callbacks.size() - 1]; // Last callback of last handler
}

Engine::Input::RangedHandler::RangedCallback* Engine::Input::RegisterRangedCallback(char key1, char key2, std::function<void()> callback)
{
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < RangedHandler::handlers.size(); i++)
	{
		if (RangedHandler::handlers[i]->key1 == key1 && RangedHandler::handlers[i]->key2 == key2)
		{
			RangedHandler::handlers[i]->callbacks.push_back(new RangedHandler::RangedCallback(callback, RangedHandler::handlers[i]));
			return RangedHandler::handlers[i]->callbacks[RangedHandler::handlers[i]->callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	RangedHandler::handlers.push_back(new RangedHandler(key1, key2));
	// And add a callback to it
	RangedHandler::handlers[i]->callbacks.push_back(new RangedHandler::RangedCallback(callback, RangedHandler::handlers[i]));
	return RangedHandler::handlers[i]->callbacks[RangedHandler::handlers[i]->callbacks.size() - 1]; // Last callback
}

void Engine::Input::Call()
{
	// Update groups' callbacks' `enabled` if `synced` is false
	for (CallbacksGroup*& group : CallbacksGroup::groups)
	{
		if (!group->synced)
		{
			for (BasicHandler::BasicCallback*& callback : group->basicCallbacks)
				callback->enabled = group->enabled;
			for (RangedHandler::RangedCallback*& callback: group->rangedCallbacks)
				callback->enabled = group->enabled;
			group->synced = true;
		}
	}
	// Call basic handlers' callbacks
	for (BasicHandler*& handler : BasicHandler::handlers)
	{
		if (handler->timesPressed > 0)
		{
			for (BasicHandler::BasicCallback*& callback : handler->callbacks)
			{
				if (callback->enabled && callback->onTick && callback->callback)
				{
					input = handler->input;
					callback->callback();
				}
			}
			handler->timesPressed = 0;
		}
	}
}

std::string& Engine::Input::Get()
{
	static char* buf = new char[maxInputLength];

	// Clear previous buffer
	memset(buf, 0, maxInputLength);

	// Read to buffer (blocking)
	read(0, buf, maxInputLength);

	// Update `std::string Input::input`
	input.assign(buf);

	// Call basic handlers
	for (size_t i = 0; i < BasicHandler::handlers.size(); i++)
	{
		if (input == BasicHandler::handlers[i]->input) // If the strings are equal
		{
			BasicHandler::handlers[i]->timesPressed++;
			for (size_t j = 0; j < BasicHandler::handlers[i]->callbacks.size(); j++) // Call the calls
				if (BasicHandler::handlers[i]->callbacks[j]->enabled && !BasicHandler::handlers[i]->callbacks[j]->onTick) // Check if the callback is enabled and shouldn't be called on tick
					BasicHandler::handlers[i]->callbacks[j]->callback(); // Call
			break; // Break because there shouldn't be a similar handler
		}
	}
	// Call ranged handlers, only if the input is 1 letter in length
	if (input.length() == 1)
	{
		for (size_t i = 0; i < RangedHandler::handlers.size(); i++)
		{
			if (RangedHandler::handlers[i]->key1 <= input[0] && input[0] <= RangedHandler::handlers[i]->key2) // If the key is in range
			{
				for (RangedHandler::RangedCallback*& callback: RangedHandler::handlers[i]->callbacks)
					if (callback->enabled) // Check if the callback is enabled
						callback->callback(); // Call
				// Don't break since there can be more handlers which manage an area in which the input is located in
			}
		}
	}
	return input;
}

void Engine::Input::Loop()
{
	while (Engine::running)
		Get();
}

bool Engine::Input::Is(const std::string& stringKey)
{
	return (input == stringKey);
}

bool Engine::Input::Is(char charKey)
{
	return input.length() == 1 && input[0] == charKey;
}

bool Engine::Input::IsNum()
{
	return (input[0] >= '0') && (input[0] <= '9') && (input.length() == 1);
}

uint8_t Engine::Input::GetNum()
{
	return input[0] - '0';
}

bool Engine::Input::Bigger(char charKey)
{
	return (input[0] >= charKey) && (input[1] == 0);
}

bool Engine::Input::Smaller(char charKey)
{
	return (input[0] <= charKey) && (input[1] == 0);
}

bool Engine::Input::Between(char charKey1, char charKey2)
{
	return (input[0] >= charKey1) && (input[0] <= charKey2) && (input[1] == 0);
}

void Engine::Input::CallbacksGroup::DeleteCallbacks()
{
	// Basic handlers
	for (BasicHandler::BasicCallback*& callback : basicCallbacks)
	{
		// Access all of the callbacks' parent handlers
		for (size_t i = 0; i < callback->parentHandler->callbacks.size(); i++)
		{
			// Find the callback's pointer within its parent handler and erase it from the handler's vector
			if (callback == callback->parentHandler->callbacks[i])
				callback->parentHandler->callbacks.erase(callback->parentHandler->callbacks.begin() + i);
		}
		// Delete the callback itself
		delete callback;
	}
	// Clear the callbacks vector within the group
	basicCallbacks.clear();
	// Ranged handlers
	for (RangedHandler::RangedCallback*& callback : rangedCallbacks)
	{
		// Access all of the callbacks' parent handlers
		for (size_t i = 0; i < callback->parentHandler->callbacks.size(); i++)
		{
			// Find the callback's pointer within its parent handler and erase it from the handler's vector
			if (callback == callback->parentHandler->callbacks[i])
				callback->parentHandler->callbacks.erase(callback->parentHandler->callbacks.begin() + i);
		}
		// Delete the callback itself
		delete callback;
	}
	// Clear the callbacks vector within the group
	rangedCallbacks.clear();
}