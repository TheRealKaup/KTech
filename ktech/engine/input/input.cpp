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

#include "input.hpp"

#include "callbacks_handlers.hpp"
#include "callbacksgroup.hpp"
#include "../engine.hpp"

#include <cstring>
#include <unistd.h>

#define maxInputLength 7

KTech::Input::Input(Engine* const p_engine)
	: engine(p_engine)
{
	// (INPUT) Set terminal attributes
	tcgetattr(0, &m_oldTerminalAttributes);
	termios terminalAttributes = m_oldTerminalAttributes;
	terminalAttributes.c_lflag &= ~ICANON; // Disable canonical mode
	terminalAttributes.c_lflag &= ~ECHO; // Disable echo
	terminalAttributes.c_lflag &= ~ISIG; // Disable signals
	terminalAttributes.c_cc[VMIN] = 1; // Blocking read
	terminalAttributes.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &terminalAttributes);

	// (INPUT) Create input loop
	m_inputLoop = std::thread(std::bind(&Input::Loop, this));
}

KTech::Input::~Input()
{
	// Return to the old terminal attributes
	tcsetattr(0, TCSANOW, &m_oldTerminalAttributes);
	
	// Detach so in the case that the game quit regardless of player input,
	// the input loop thread would end.
	m_inputLoop.detach();
}

KTech::Input::BasicCallback* KTech::Input::RegisterCallback(const std::string& p_input, const std::function<void()>& p_callback, bool p_onTick)
{
	if (p_callback == nullptr) // Avoid constantly checking later whether callback is null
		return nullptr;
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < m_basicHandlers.size(); i++)
	{
		if (m_basicHandlers[i]->m_input == p_input)
		{
			m_basicHandlers[i]->m_callbacks.push_back(new BasicCallback(p_callback, m_basicHandlers[i], p_onTick));
			return m_basicHandlers[i]->m_callbacks[m_basicHandlers[i]->m_callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	m_basicHandlers.push_back(new BasicHandler(p_input));
	// And add a callback to it
	m_basicHandlers[i]->m_callbacks.push_back(new BasicCallback(p_callback, m_basicHandlers[i], p_onTick));
	return m_basicHandlers[i]->m_callbacks[m_basicHandlers[i]->m_callbacks.size() - 1]; // Last callback of last handler
}

KTech::Input::RangedCallback* KTech::Input::RegisterRangedCallback(char p_key1, char p_key2, const std::function<void()>& p_callback)
{
	if (p_callback == nullptr) // Avoid constantly checking later whether callback is null
		return nullptr;
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < m_rangedHandlers.size(); i++)
	{
		if (m_rangedHandlers[i]->m_key1 == p_key1 && m_rangedHandlers[i]->m_key2 == p_key2)
		{
			m_rangedHandlers[i]->m_callbacks.push_back(new RangedCallback(p_callback, m_rangedHandlers[i]));
			return m_rangedHandlers[i]->m_callbacks[m_rangedHandlers[i]->m_callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	m_rangedHandlers.push_back(new RangedHandler(p_key1, p_key2));
	// And add a callback to it
	m_rangedHandlers[i]->m_callbacks.push_back(new RangedCallback(p_callback, m_rangedHandlers[i]));
	return m_rangedHandlers[i]->m_callbacks[m_rangedHandlers[i]->m_callbacks.size() - 1]; // Last callback
}

KTech::Input::CallbacksGroup* KTech::Input::CreateCallbacksGroup(bool p_enabled)
{
	m_groups.push_back(new CallbacksGroup(p_enabled));
	return m_groups[m_groups.size() - 1];
}

bool KTech::Input::Is(const std::string& p_stringKey)
{
	return (input == p_stringKey);
}

bool KTech::Input::Is(char p_charKey)
{
	return input.length() == 1 && input[0] == p_charKey;
}

uint8_t KTech::Input::GetInt()
{
	return input[0] - '0';
}

bool KTech::Input::Bigger(char p_charKey)
{
	return (input[0] >= p_charKey) && (input[1] == 0);
}

bool KTech::Input::Smaller(char p_charKey)
{
	return (input[0] <= p_charKey) && (input[1] == 0);
}

bool KTech::Input::Between(char p_charKey1, char p_charKey2)
{
	return (input[0] >= p_charKey1) && (input[0] <= p_charKey2) && (input[1] == 0);
}

void KTech::Input::CallHandlers()
{
	// Update groups' callbacks' `enabled` if `synced` is false
	for (CallbacksGroup*& group : m_groups)
	{
		group->Update();
	}
	// Call basic handlers' callbacks
	for (BasicHandler*& handler : m_basicHandlers)
	{
		if (handler->m_timesPressed > 0)
		{
			for (BasicCallback*& callback : handler->m_callbacks)
			{
				if (callback->enabled && callback->onTick)
				{
					input = handler->m_input; // Would be preferable to not copy it this way each iteration
					callback->ptr();
				}
			}
			handler->m_timesPressed = 0;
		}
	}
}

char* KTech::Input::Get()
{
	static char* buf = new char[maxInputLength];
	
	// Clear previous buffer
	memset(buf, 0, maxInputLength);
	// Read input to buffer (blocking)
	read(0, buf, maxInputLength);

	return buf;
}

void KTech::Input::Loop()
{
	while (engine->running)
	{
		// Get input and update `std::string Input::input`
		input.assign(Get());
		inputThisTick = true;
		// Quit
		if (input == quitKey)
		{
			engine->running = false;
			break;
		}
		// Call basic handler
		for (size_t i = 0; i < m_basicHandlers.size(); i++)
		{
			if (input == m_basicHandlers[i]->m_input) // The input is of this handler, call handler's callbacks
			{
				m_basicHandlers[i]->m_timesPressed++; // Used in synced calling
				for (size_t j = 0; j < m_basicHandlers[i]->m_callbacks.size(); j++) // Call the calls
					if (m_basicHandlers[i]->m_callbacks[j]->enabled && !m_basicHandlers[i]->m_callbacks[j]->onTick) // Check if the callback shouldn't be called on tick
						m_basicHandlers[i]->m_callbacks[j]->ptr(); // Call
				break; // Break because there shouldn't be a similar handler
			}
		}
		// Call ranged handler, only if the input is a single character
		if (input.length() == 1)
			for (size_t i = 0; i < m_rangedHandlers.size(); i++)
				if (m_rangedHandlers[i]->m_key1 <= input[0] && input[0] <= m_rangedHandlers[i]->m_key2) // If the key is in range
					for (RangedCallback*& callback: m_rangedHandlers[i]->m_callbacks)
						if (callback->enabled)
							callback->ptr(); // Call
	}
}