#include "engine.hpp"
#include <ostream>
#include <sys/types.h>

// read https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences

void Engine::GetInput()
{
	for (unsigned i = 0; i < 8; i++)
		Input::input[i] = 0;
	read(0, &Input::input, 8);
}
