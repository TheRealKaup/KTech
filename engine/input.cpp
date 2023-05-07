#include "engine.hpp"

// read https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

// The input as the engine recieved it, including escape codes.
char Engine::Input::input[8] = {0};
// The length (in bytes) of the input as recieved by the engine.
unsigned char Engine::Input::inputLength = 0;
// The key in the input, unmodified (shift doesn't change the value).
unsigned char Engine::Input::inputKey = 0;
// The key in the input, modified by shift.
unsigned char Engine::Input::modifiedInputKey = 0;
// The alt modifier key state
bool Engine::Input::alt = false;
// The control modifier key state
bool Engine::Input::ctrl = false;

struct InputHandler {
	std::function<void> call;
	// This vector has strings of inputs. The reason inputs is plural, is that the input could come with an alt escape sequence that isn't relevant.
	// There is probably a better way of doing this.
	std::vector<char*> inputs; 
};
std::vector<InputHandler> handlers;

// Register a input handler which will call a function when the key is pressed 
void Engine::Input::RegisterInputHandler(unsigned char key, char mode, std::function<void> call) {
	handlers.push_back({call, key, mode});

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

void Engine::GetInput()
{
	for (unsigned i = 0; i < 8; i++)
		Input::input[i] = 0;
	read(0, &Input::input, 8);
	// process input, call the correct input handler
	// or instead, process input only in RegisterInputHandler and here simpy compare input strings
}
