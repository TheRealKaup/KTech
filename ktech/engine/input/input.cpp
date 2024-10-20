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

KTech::Input::Callback* KTech::Input::RegisterCallback(const std::string& p_input, const std::function<bool()>& p_callback)
{
	if (p_callback == nullptr) // Avoid constantly checking later whether callback is null
		return nullptr;
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < m_handlers.size(); i++)
	{
		if (m_handlers[i]->m_input == p_input)
		{
			m_handlers[i]->m_callbacks.push_back(new Callback(p_callback, m_handlers[i]));
			return m_handlers[i]->m_callbacks[m_handlers[i]->m_callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	m_handlers.push_back(new Handler(p_input));
	// And add a callback to it
	m_handlers[i]->m_callbacks.push_back(new Callback(p_callback, m_handlers[i]));
	return m_handlers[i]->m_callbacks[m_handlers[i]->m_callbacks.size() - 1]; // Last callback of last handler
}

KTech::Input::Callback* KTech::Input::RegisterRangedCallback(char p_key1, char p_key2, const std::function<bool()>& p_callback)
{
	if (p_callback == nullptr) // Avoid constantly checking later whether callback is null
		return nullptr;
	// If a handler already exists for this input, add the callback to the calls vector
	size_t i = 0; // Creating it out of the for's scope so I can use it as size of `handlers` later
	for (; i < m_handlers.size(); i++)
	{
		if (m_handlers[i]->m_key1 == p_key1 && m_handlers[i]->m_key2 == p_key2)
		{
			m_handlers[i]->m_callbacks.push_back(new Callback(p_callback, m_handlers[i]));
			return m_handlers[i]->m_callbacks[m_handlers[i]->m_callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	m_handlers.push_back(new Handler(p_key1, p_key2));
	// And add a callback to it
	m_handlers[i]->m_callbacks.push_back(new Callback(p_callback, m_handlers[i]));
	return m_handlers[i]->m_callbacks[m_handlers[i]->m_callbacks.size() - 1]; // Last callback
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
		input = trigger;
		if (trigger.length() == 1) // Can be both string/range handler
		{
			for (Handler* handler : m_handlers)
			{
				if (handler->m_type == Handler::Type::String) // String handler
				{
					if (trigger == handler->m_input)
					{
						// Call callbacks
						for (Callback* callback : handler->m_callbacks)
							if (callback->enabled && callback->ptr())
								changedThisTick = true; // Render-on-demand
					}
				}
				else if (handler->m_key1 <= trigger[0] && trigger[0] <= handler->m_key2) // Range handler
					for (Callback* callback: handler->m_callbacks)
						if (callback->enabled && callback->ptr())
							changedThisTick = true; // Render-on-demand
			}
		}
		else // Can only be string handler
		{
			for (Handler* handler : m_handlers)
			{
				if (handler->m_type == Handler::Type::String // String handler
					&& trigger == handler->m_input)
				{
					// Call callbacks
					for (Callback* callback : handler->m_callbacks)
						if (callback->enabled && callback->ptr())
							changedThisTick = true; // Render-on-demand
					break; // Break because there shouldn't be a similar handler
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
	// Add space in history
	m_triggers.push_back(std::move(std::string()));
	// Write input to history (TCHAR to char conversion)
	LPDWORD nReadCharacters = 0;
	TCHAR buf[7];
	memset(buf, 0, sizeof(buf));
	ReadConsole(m_stdinHandle, buf.data(), buf.length(), (LPDWORD)(&nReadCharacters), NULL);
	m_triggers[m_triggers.size() - 1].resize(nReadCharacters);
	for (size_t i = 0; i < nReadCharacters; i++)
		m_triggers[m_triggers.size() - 1] += buf[i];
#else
	std::string buf(7, '\0');
	buf.resize(read(0, buf.data(), buf.size()));
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