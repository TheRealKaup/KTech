#include "engine.hpp"

#define maxInputLength 7

size_t Engine::Input::handlerIndex = -1;

std::string Engine::Input::input(maxInputLength, '\0');

Engine::Input::Handler::Handler(const std::string& input, std::function<void()> callback, bool onTick) : input(input)
{
	// So the pointers that groups have stay correct
	callbacks.push_back(new Handler::Callback(callback, onTick, this));
}

Engine::Input::Handler::Callback* Engine::Input::RegisterCallback(const std::string& input, std::function<void()> callback, bool onTick)
{
	// If a handler already exists for this input, add the callback to the calls vector
	for (size_t i = 0; i < Handler::handlers.size(); i++)
	{
		if (Handler::handlers[i]->input == input)
		{
			Handler::handlers[i]->callbacks.push_back(new Handler::Callback(callback, onTick, Handler::handlers[i]));
			return Handler::handlers[i]->callbacks[Handler::handlers[i]->callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	Handler::handlers.push_back(new Handler(input, callback, onTick));
	return Handler::handlers[Handler::handlers.size() - 1]->callbacks[Handler::handlers[Handler::handlers.size() - 1]->callbacks.size() - 1]; // Last callback of last handler
}

uint32_t Engine::Input::Call()
{
	for (CallbacksGroup*& group : CallbacksGroup::groups)
	{
		if (!group->synced)
		{
			for (Handler::Callback*& callback : group->callbacks)
				callback->enabled = group->enabled;
			group->synced = true;
		}
	}
	uint32_t counter = 0;
	for (Handler*& handler : Handler::handlers)
	{
		if (handler->timesPressed > 0)
		{
			for (Handler::Callback*& callback : handler->callbacks)
			{
				if (callback->enabled && callback->onTick && callback->callback)
				{
					counter++;
					input = handler->input;
					callback->callback();
				}
			}
			handler->timesPressed = 0;
		}
	}
	return counter;
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

	// Call handlers
	for (size_t i = 0; i < Handler::handlers.size(); i++)
	{
		if (input == Handler::handlers[i]->input) // If the strings are equal
		{
			Handler::handlers[i]->timesPressed++;
			for (size_t j = 0; j < Handler::handlers[i]->callbacks.size(); j++) // Call the calls
				if (Handler::handlers[i]->callbacks[j]->enabled && !Handler::handlers[i]->callbacks[j]->onTick && Handler::handlers[i]->callbacks[j]->callback) // Check if the call shouldn't be called on tick and isn't null
					Handler::handlers[i]->callbacks[j]->callback(); // Call
			break;
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
	for (Handler::Callback*& callback : callbacks)
	{
		// Access all of the callbacks' parent handlers
		for (size_t i = 0; i < callback->handlerParent->callbacks.size(); i++)
		{
			// Find the callback's pointer within its parent handler and erase it from the handler's vector
			if (callback == callback->handlerParent->callbacks[i])
				callback->handlerParent->callbacks.erase(callback->handlerParent->callbacks.begin() + i);
		}
		// Delete the callback itself
		delete callback;
	}
	// Clear the callbacks vector within the group
	callbacks.clear();
}