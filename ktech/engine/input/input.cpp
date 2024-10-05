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
#ifndef _WIN32
#include <unistd.h>
#endif

KTech::Input::Input(Engine* const p_engine)
	: engine(p_engine)
{
	// Set terminal attributes
#ifdef _WIN32
	m_stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(m_stdinHandle, &m_oldMode);
	SetConsoleMode(m_stdinHandle,
		// Enable:
		(m_oldMode
		| ENABLE_VIRTUAL_TERMINAL_INPUT // Receive input as escape sequences
		| ENABLE_EXTENDED_FLAGS) // Needed to disable selecting text (because it pauses output)
		// Disable:
		& ~(ENABLE_ECHO_INPUT // Echo
		| ENABLE_LINE_INPUT // Canonical mode
		| ENABLE_PROCESSED_INPUT // System handling of signals (e.g. Ctrl+C)
		| ENABLE_MOUSE_INPUT // Inputs regarding the mouse
		| ENABLE_WINDOW_INPUT // Input regarding the window
		| ENABLE_QUICK_EDIT_MODE) // Selecting text
	);
	SetConsoleCP(20127); // us-ascii code page (list of code pages: https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers)
#else
	tcgetattr(0, &m_oldTerminalAttributes);
	termios terminalAttributes = m_oldTerminalAttributes;
	terminalAttributes.c_lflag &= ~ICANON; // Disable canonical mode
	terminalAttributes.c_lflag &= ~ECHO; // Disable echo
	terminalAttributes.c_lflag &= ~ISIG; // Disable signals
	terminalAttributes.c_cc[VMIN] = 1; // Blocking read
	terminalAttributes.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &terminalAttributes);
#endif

	// Create the input loop
	m_inputLoop = std::thread(std::bind(&Input::Loop, this));
}

KTech::Input::~Input()
{
	// Return to the old terminal attributes
#if _WIN32
	SetConsoleMode(m_stdinHandle, m_oldMode);
#else
	tcsetattr(0, TCSANOW, &m_oldTerminalAttributes);
#endif

	// Detach so in the case that the game quit regardless of player input,
	// the input loop thread would end.
	m_inputLoop.detach();
}

KTech::Input::BasicCallback* KTech::Input::RegisterCallback(const std::string& p_input, const std::function<bool()>& p_callback, bool p_onTick)
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

KTech::Input::RangedCallback* KTech::Input::RegisterRangedCallback(char p_key1, char p_key2, const std::function<bool()>& p_callback)
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

bool KTech::Input::Is(const std::string& p_stringKey) const
{
	return (input == p_stringKey);
}

bool KTech::Input::Is(char p_charKey) const
{
	return input.length() == 1 && input[0] == p_charKey;
}

uint8_t KTech::Input::GetInt() const
{
	return input[0] - '0';
}

bool KTech::Input::Bigger(char p_charKey) const
{
	return (input[0] >= p_charKey) && (input[1] == 0);
}

bool KTech::Input::Smaller(char p_charKey) const
{
	return (input[0] <= p_charKey) && (input[1] == 0);
}

bool KTech::Input::Between(char p_charKey1, char p_charKey2) const
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
					if (callback->ptr())
						changedThisTick = true; // Render-on-demand
				}
			}
			handler->m_timesPressed = 0;
		}
	}
}

void KTech::Input::Get()
{
#ifdef _WIN32
	typedef TCHAR BufChar;
#else
	typedef char BufChar;
#endif
	BufChar buf[7];

	// Clear previous buffer
	memset(buf, 0, sizeof(buf));
	// Read input to buffer (blocking)
#ifdef _WIN32
	LPDWORD nReadCharacters = 0;
	ReadConsole(m_stdinHandle, buf, sizeof(buf) / sizeof(BufChar), (LPDWORD)(&nReadCharacters), NULL);
	input.clear();
	for (size_t i = 0; buf[i]; i++)
		input += buf[i];
#else
	read(0, buf, sizeof(buf) / sizeof(BufChar));
	input.assign(buf);
#endif
}

void KTech::Input::Loop()
{
	while (engine->running)
	{
		// Get input
		Get();
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
						if (m_basicHandlers[i]->m_callbacks[j]->ptr()) // Call
							changedThisTick = true; // Render-on-demand
				break; // Break because there shouldn't be a similar handler
			}
		}
		// Call ranged handler, only if the input is a single character
		if (input.length() == 1)
			for (size_t i = 0; i < m_rangedHandlers.size(); i++)
				if (m_rangedHandlers[i]->m_key1 <= input[0] && input[0] <= m_rangedHandlers[i]->m_key2) // If the key is in range
					for (RangedCallback*& callback: m_rangedHandlers[i]->m_callbacks)
						if (callback->enabled)
							if (callback->ptr()) // Call
								changedThisTick = true; // Render-on-demand
	}
}