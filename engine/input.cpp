#include "engine.hpp"

struct InputHandler
{
	std::vector<std::function<void()>> functions;
	bool down = false;
	bool onDown;
	int key;
	int lastKeyRepresentation; // is the user registers the key as ',' instead of VK_OEM_COMMA, then they would probably want lastKey to be set as ',' and not a virutal key code.
	bool requiresShift;

	InputHandler(int win32Key, bool onKeyDown, std::function<void()> function, int lastKeyRepresentation, bool requiresShift)
		: key(win32Key), onDown(onKeyDown), functions({ function }), lastKeyRepresentation(lastKeyRepresentation), requiresShift(requiresShift) {}
};

std::vector<InputHandler> inputHandlers;

/*
void Engine::RegisterInputHandler(int key, bool onKeyDown, std::function<void()> function, bool vk)
{
	if (!function)
		return;

	int convertedKey = key;
	bool requiresShift = false;
	
	if (!vk) // Microsoft are so stupid they set virtual keys like VK_UP to = '&'.
	{
		if (key >= 'A' && key <= 'Z')
			requiresShift = true;
		else if (key >= 'a' && key <= 'z')
			convertedKey = key - 32;
		else if (key == '!')
		{
			requiresShift = true;
			convertedKey = '1';
		}
		else if (key == '@')
		{
			requiresShift = true;
			convertedKey = '2';
		}
		else if (key == '#')
		{
			requiresShift = true;
			convertedKey = '3';
		}
		else if (key == '$')
		{
			requiresShift = true;
			convertedKey = '4';
		}
		else if (key == '%')
		{
			requiresShift = true;
			convertedKey = '5';
		}
		else if (key == '^')
		{
			requiresShift = true;
			convertedKey = '6';
		}
		else if (key == '&')
		{
			requiresShift = true;
			convertedKey = '7';
		}
		else if (key == '*')
		{
			requiresShift = true;
			convertedKey = '8';
		}
		else if (key == '(')
		{
			requiresShift = true;
			convertedKey = '9';
		}
		else if (key == ')')
		{
			requiresShift = true;
			convertedKey = '0';
		}
		else if (key == ' ')
			convertedKey = VK_SPACE;
		else if (key == '`')
			convertedKey = VK_OEM_3;
		else if (key == '~')
		{
			requiresShift = true;
			convertedKey = VK_OEM_3;
		}
		else if (key == '-')
			convertedKey = VK_OEM_MINUS;
		else if (key == '_')
		{
			requiresShift = true;
			convertedKey = VK_OEM_MINUS;
		}
		else if (key == '=')
			convertedKey = VK_OEM_PLUS;
		else if (key == '+')
		{
			requiresShift = true;
			convertedKey = VK_OEM_PLUS;
		}
		else if (key == '\\')
			convertedKey = VK_OEM_5;
		else if (key == '|')
		{
			requiresShift = true;
			convertedKey = VK_OEM_5;
		}
		else if (key == '[')
			convertedKey = VK_OEM_4;
		else if (key == '{')
		{
			requiresShift = true;
			convertedKey = VK_OEM_4;
		}
		else if (key == ']')
			convertedKey = VK_OEM_6;
		else if (key == '}')
		{
			requiresShift = true;
			convertedKey = VK_OEM_6;
		}
		else if (key == ';')
			convertedKey = VK_OEM_1;
		else if (key == ':')
		{
			requiresShift = true;
			convertedKey = VK_OEM_1;
		}
		else if (key == '\'')
			convertedKey = VK_OEM_7;
		else if (key == '"')
		{
			requiresShift = true;
			convertedKey = VK_OEM_7;
		}
		else if (key == ',')
			convertedKey = VK_OEM_COMMA;
		else if (key == '<')
		{
			requiresShift = true;
			convertedKey = VK_OEM_COMMA;
		}
		else if (key == '.')
			convertedKey = VK_OEM_PERIOD;
		else if (key == '>')
		{
			requiresShift = true;
			convertedKey = VK_OEM_PERIOD;
		}
		else if (key == '/')
			convertedKey = VK_OEM_2;
		else if (key == '?')
		{
			requiresShift = true;
			convertedKey = VK_OEM_2;
		}
	}
	
	// If the InputHandler already exists, add the function to the functions vector and return. No need to create another InputHandler.
	for (size_t i = 0; i < inputHandlers.size(); i++)
	{
		if (inputHandlers[i].key == convertedKey && inputHandlers[i].lastKeyRepresentation == key && inputHandlers[i].onDown == onKeyDown)
		{
			inputHandlers[i].functions.push_back(function);
			return;
		}
	}

	inputHandlers.push_back(InputHandler(convertedKey, onKeyDown, function, key, requiresShift));
}
*/
void Engine::ManageInputs()
{	
	int startingHandlersAmount = inputHandlers.size();
	bool currentKeyState = false;
	
	for (int i = 0; i < inputHandlers.size(); i++)
	{
		currentKeyState = false; // GetAsyncKeyState(inputHandlers[i].key);

		if (inputHandlers[i].onDown && !inputHandlers[i].down && currentKeyState)
		{
			Engine::lastDownKey = inputHandlers[i].lastKeyRepresentation;
			Engine::lastDownVirtualKey = inputHandlers[i].key;
			Engine::lastDownKeyRequiresShift = inputHandlers[i].requiresShift;
			for (size_t j = 0; j < inputHandlers[i].functions.size(); j++)
				inputHandlers[i].functions[j]();
		}
		if (!inputHandlers[i].onDown && inputHandlers[i].down && !currentKeyState)
		{
			Engine::lastUpKey = inputHandlers[i].lastKeyRepresentation;
			Engine::lastUpVirtualKey = inputHandlers[i].key;
			Engine::lastUpKeyRequiresShift = inputHandlers[i].requiresShift;
			for (size_t j = 0; j < inputHandlers[i].functions.size(); j++)
				inputHandlers[i].functions[j]();
		}
		
		if (inputHandlers.size() != startingHandlersAmount)
			return;

		inputHandlers[i].down = currentKeyState;
	}
}
