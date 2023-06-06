#include "engine.hpp"

#define maxEscapeSequenceLength 7

char* Engine::Input::buf = new char[maxEscapeSequenceLength];
unsigned char Engine::Input::length = 0;
char* Engine::Input::quitString = (char*)"\03";

void Engine::Input::Handler::AddCall(std::function<void()> call, bool onTick)
{
	onTicks.push_back(onTick);
	calls.push_back(call);
}
Engine::Input::Handler::Handler(std::string input, std::function<void()> call, bool onTick) : input(input)
{
	onTicks.push_back(onTick);
	calls.push_back(call);
}
std::vector<Engine::Input::Handler> Engine::Input::handlers = {};
unsigned Engine::Input::handlerIndex = -1;

void Engine::Input::RegisterHandler(std::string input, std::function<void()> call, bool onTick)
{
	// If a handler already exists for this input, add the call to the calls vector
	for (unsigned i = 0; i < handlers.size(); i++)
		if (handlers[i].input == input)
		{
			handlers[i].AddCall(call, onTick);
			return;
		}
	handlers.push_back(Handler(input, call, onTick));
}
void Engine::Input::RegisterHandler(char input, std::function<void()> call, bool onTick)
{
	// If a handler already exists for this input, add the call to the calls vector
	for (unsigned i = 0; i < handlers.size(); i++)
		if (handlers[i].input.length() == 1 && handlers[i].input[0] == input)
		{
			handlers[i].AddCall(call, onTick);
			return;
		}
	handlers.push_back(Handler({input}, call, onTick));
}

// The issue: Call() doesn't set buf accordingly

void Engine::Input::Call()
{
	for (unsigned i = 0; i < handlers.size(); i++)
		if (handlers[i].timesPressed > 0)
		{
			for (unsigned j = 0; j < handlers[i].calls.size(); j++)
				if (handlers[i].onTicks[j] && handlers[i].calls[j])
					handlers[i].calls[j]();
			handlers[i].timesPressed = 0;
		}
}

char* Engine::Input::Get()
{
	// Clear previous buffer
	memset(buf, 0, maxEscapeSequenceLength);

	// Read to buffer (blocking)
	length = read(0, buf, maxEscapeSequenceLength);

	// Call handlers
	for (unsigned i = 0; i < handlers.size(); i++)
		if (strcmp(buf, handlers[i].input.c_str()) == 0) // If the strings are equal
		{
			handlers[i].timesPressed++;
			for (unsigned j = 0; j < handlers[i].calls.size(); j++) // Call the calls
				if (!handlers[i].onTicks[j] && handlers[i].calls[j]) // Check if the call shouldn't be called on tick and isn't null
					handlers[i].calls[j](); // Call
		}
	
	return buf;
}

void Engine::Input::Loop()
{
	// `strcmp(Get(), "\03") != 0` - Quit if Ctrl+C was received 
	while (strcmp(Get(), quitString) != 0);
}

bool Engine::Input::Is(std::string stringKey)
{
	return (strcmp(buf, stringKey.c_str()) == 0);
}

bool Engine::Input::Is(char charKey)
{
	return (buf[0] == charKey) && (buf[1] == 0);
}

bool Engine::Input::IsNum()
{
	return (buf[0] >= '0') && (buf[0] <= '9') && (buf[1] == 0);
}

unsigned char Engine::Input::Num()
{
	return (buf[0] - '0');
}

bool Engine::Input::Bigger(char charKey)
{
	return (buf[0] >= charKey) && (buf[1] == 0);
}

bool Engine::Input::Smaller(char charKey)
{
	return (buf[0] <= charKey) && (buf[1] == 0);
}

bool Engine::Input::Between(char charKey1, char charKey2)
{
	return (buf[0] >= charKey1) && (buf[0] <= charKey2) && (buf[1] == 0);
}