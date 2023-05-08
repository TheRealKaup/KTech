#include "engine.hpp"

#define maxEscapeSequenceLength 7

char* Engine::Input::buf = new char[maxEscapeSequenceLength];
unsigned char Engine::Input::length = 0;

struct InputHandler
{
	std::function<void()> call;
	char* input;
};
std::vector<InputHandler> handlers;

// Register a input handler which will call a function when the key is pressed 
bool Engine::Input::RegisterHandler(char* input, std::function<void()> call)
{
	handlers.push_back({call, input});
	return true;
	// 3 main options:
	// 	1) call on key that requires shift ('A', '#', '?') only when shift IS down
	// 	2) call on key that doesn't require shift ('a', '3', '/') only when shift is NOT down
	// 	3) call on key no matter if it requires shift ('A', 'a', '#', '3', '?', '/'), no matter is shift is down
	// To register option number...:
	// 	1) "key" parameter should be modified by shift, and "shift" should be true
	// 	2) "key" should be unmodified by shift, and "shift" should be false
	// 	3) "key" should be unmodified or modified by shift, and "shift" should be true or false, respectively
	// Or, "shift" will be an unsigned char and "key" can be modified or unmodified, and:
	// 	1) "shift" == true
	// 	2) "shift" == false
	// 	3) "shift" == doesn't matter
	// With this technique, both ctrl and alt can be compressed into a single byte with bitwise operations.
	// For example:
	// 	Shift_False | Ctrl_Irrelevant | Alt_False
}

void Engine::Input::Get()
{
	// Clear previous buffer
	for (unsigned i = 0; i < maxEscapeSequenceLength; i++)
		buf[i] = 0;
	// Read to buffer (blocking)
	length = read(0, &buf, maxEscapeSequenceLength);
	// Call handlers
	for (unsigned i = 0; i < handlers.size(); i++)
		if (std::strcmp(buf, handlers[i].input) == 0) // If the strings are equal
			if (handlers[i].call) // If the call is not null
				handlers[i].call(); // Call the handler's function
}

void Engine::Input::Loop()
{
	while (running)
		Get();
}