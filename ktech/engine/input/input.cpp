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

#include "callback.hpp"
#include "callbacksgroup.hpp"
#include "handler.hpp"
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

KTech::Input::Callback* KTech::Input::RegisterCallback(const std::string& p_input, const std::function<bool()>& p_callback)
{
	if (p_callback == nullptr) // Avoid constantly checking later whether callback is null
		return nullptr;
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < m_stringHandlers.size(); i++)
	{
		if (m_stringHandlers[i]->m_string == p_input)
		{
			m_stringHandlers[i]->m_callbacks.push_back(new Callback(p_callback, m_stringHandlers[i]));
			return m_stringHandlers[i]->m_callbacks[m_stringHandlers[i]->m_callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	m_stringHandlers.push_back(new Handler(p_input));
	// And add a callback to it
	m_stringHandlers[i]->m_callbacks.push_back(new Callback(p_callback, m_stringHandlers[i]));
	return m_stringHandlers[i]->m_callbacks[m_stringHandlers[i]->m_callbacks.size() - 1]; // Last callback of last handler
}

KTech::Input::Callback* KTech::Input::RegisterRangedCallback(char p_key1, char p_key2, const std::function<bool()>& p_callback)
{
	if (p_callback == nullptr) // Avoid constantly checking later whether callback is null
		return nullptr;
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < m_rangeHandlers.size(); i++)
	{
		if (m_rangeHandlers[i]->m_start == p_key1 && m_rangeHandlers[i]->m_end == p_key2)
		{
			m_rangeHandlers[i]->m_callbacks.push_back(new Callback(p_callback, m_rangeHandlers[i]));
			return m_rangeHandlers[i]->m_callbacks[m_rangeHandlers[i]->m_callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	m_rangeHandlers.push_back(new Handler(p_key1, p_key2));
	// And add a callback to it
	m_rangeHandlers[i]->m_callbacks.push_back(new Callback(p_callback, m_rangeHandlers[i]));
	return m_rangeHandlers[i]->m_callbacks[m_rangeHandlers[i]->m_callbacks.size() - 1]; // Last callback
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

void KTech::Input::CallCallbacks()
{
	// Update groups' callbacks' `enabled` if `synced` is false
	for (CallbacksGroup*& group : m_groups)
		if (!group->m_synced)
			group->Update();
	
	// Call callbacks of triggered handlers
	for (std::string& trigger : m_triggers)
	{
		input = std::move(trigger);
		if (input.length() == 1) // Can be both string/range handler
		{
			for (Handler* stringHandler : m_stringHandlers) // Input matches handler's string
			{
				if (input == stringHandler->m_string) // String matches
				{
					// Call callbacks
					for (Callback* callback : stringHandler->m_callbacks)
						if (callback->enabled && callback->ptr()) // Call if enabled, if returns true update render-on-demand status
							changedThisTick = true; // Render-on-demand
					// Break because there shouldn't be a similar handler
					break;
				}
			}
			for (Handler* rangeHandler : m_rangeHandlers) // Range handlers
				if (rangeHandler->m_start <= input[0] && input[0] <= rangeHandler->m_end) // Input matches handler's range
					for (Callback* callback: rangeHandler->m_callbacks) // Call callbacks
						if (callback->enabled && callback->ptr()) // Call if enabled, if returns true update render-on-demand status
							changedThisTick = true; // Render-on-demand
		}
		else // Can only be string handler
		{
			for (Handler* stringHandler : m_stringHandlers) // Input matches handler's string
			{
				if (input == stringHandler->m_string) // String matches
				{
					// Call callbacks
					for (Callback* callback : stringHandler->m_callbacks)
						if (callback->enabled && callback->ptr()) // Call if enabled, if returns true update render-on-demand status
							changedThisTick = true; // Render-on-demand
					// Break because there shouldn't be a similar handler
					break;
				}
			}
		}
	}

	// All callbacks of triggered handlers were called; clear trigger history
	m_triggers.clear();
}

void KTech::Input::Get()
{
#ifdef _WIN32
	// Read
	TCHAR tcharBuf[7];
	memset(tcharBuf, 0, sizeof(tcharBuf));
	LPDWORD nReadCharacters = 0;
	ReadConsole(m_stdinHandle, tcharBuf, sizeof(tcharBuf) / sizeof(TCHAR), (LPDWORD)(&nReadCharacters), NULL);
	// Convert `TCHAR` string to `char` string
	std::string charBuf((size_t)nReadCharacters, '\0');
	for (size_t i = 0; i < (size_t)nReadCharacters; i++)
		charBuf[i] = tcharBuf[i];
	// Move to input history
	m_triggers.push_back(std::move(charBuf));
#else
	// Read
	std::string buf(7, '\0');
	buf.resize(read(0, buf.data(), buf.size()));
	// Move to input history
	m_triggers.push_back(std::move(buf));
#endif
}

void KTech::Input::Loop()
{
	while (engine->running)
	{
		// Get input
		Get();
		if (m_triggers[m_triggers.size() - 1] == quitKey)
		{
			// Quit
			engine->running = false;
			break;
		}
	}
}