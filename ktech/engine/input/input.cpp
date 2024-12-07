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

#ifndef _WIN32
#include <unistd.h>
#endif

/*!
	@var `Input::input`
	@brief Input for the last-called callback function.

	Before `Input` calls your function, it will set this string to the exact input which lead to the calling of your function. It's especially useful if you have a function that can be triggered by different inputs, like a ranged callback function (created with `CallbacksGroup::RegisterRangedCallback()`): use this variable to evaluate the actual user input.
*/
/*!
	@var `Input::quitKey`
	@brief Input that if received, breaks the input loop and sets `Engine::running` to false.

	By default, it's "\x03" (Ctrl+C), which is a common quit key among terminal applications. You may change it, but don't go Vim on your players.
*/

/*!
	@fn auto KTech::Input::Is(const std::string &stringKey) const -> bool
	@brief Checks if input equals given string.

	@param stringKey String to compare with `Input::input`.

	@return True if equal, otherwise false.
*/
auto KTech::Input::Is(const std::string& p_stringKey) const -> bool
{
	return (input == p_stringKey);
}

/*!
	@fn auto KTech::Input::Is(char charKey) const -> bool
	@brief Checks if input equals given character.

	@param charKey Character to compare with `Input::input`.

	@return True if equal (and `Input::input` is 1 character long), otherwise false.
*/
auto KTech::Input::Is(char p_charKey) const -> bool
{
	return input.length() == 1 && input[0] == p_charKey;
}

/*!
	@brief Get the first character of input as a 1-digit number.

	@return The first character of `Input::input` subtracted by 48 (the character '0'). Can return a value that is not 1-digit-long, so unless only digit characters ('0'-'9') can call your function, you should consider confirming with `Input::Between()` and the arguments ('0', '9') that input is indeed a digit.

	@see `Input::Between()`
*/
auto KTech::Input::GetInt() const -> uint8_t
{
	return input[0] - '0';
}

/*!
	@fn Input::Bigger(char charKey)
	@brief Checks if given character is bigger than input.

	@param charKey Character to compare with `Input::input`.

	@return True if bigger (and `Input::input` is 1 character long), otherwise false.
*/
auto KTech::Input::Bigger(char p_charKey) const -> bool
{
	return (input[0] >= p_charKey) && (input.length() == 1);
}

/*!
	@fn auto KTech::Input::Smaller(char charKey) const -> bool
	@brief Checks if given character is smaller than input.

	@param charKey Character to compare with `Input::input`.

	@return True if smaller (and `Input::input` is 1 character long), otherwise false.
*/
auto KTech::Input::Smaller(char p_charKey) const -> bool
{
	return (input[0] <= p_charKey) && (input.length() == 1);
}

/*!
	@fn auto KTech::Input::Between(char start, char end) const -> bool
	@brief Checks if input is between range of characters.

	@param start Start of (ASCII) character range.
	@param end End of (ASCII) character range.

	@return True if between range (and `Input::input` is 1 character long), otherwise false.
*/
auto KTech::Input::Between(char p_start, char p_end) const -> bool
{
	return (input[0] >= p_start) && (input[0] <= p_end) && (input.length() == 1);
}

/*!
	@brief Distribute accumulated inputs.

	`Input` queues received inputs until this function is called. This very function is what calls your input callback functions.

	Normally placed at the start of each game loop's iteration, with the other callback-calling functions of engine components (right now `Memory::CallOnTicks()` and `Time::CallInvocations()`, though this may change). For example:

	@code{.cpp}
	// Game loop
	while (engine.running)
	{
		// Call various callback-functions
		engine.input.CallCallbacks(); // <- Distribute inputs to input callback functions
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		// Graphics (render-on-demand)
		if (engine.output.ShouldRenderThisTick())
		{
			map.Render();
			Camera.Draw();
			engine.output.Print();
		}
		else if (engine.output.ShouldPrintThisTick())
		{
			engine.output.Print();
		}

		engine.time.WaitUntilNextTick();
	}
	@endcode

	@see `Memory::CallOnTicks()`
	@see `Time::CallInvocations()`
*/
void KTech::Input::CallCallbacks()
{
	Update();
	// Protect `std::vector m_inputQueue`
	std::lock_guard<std::mutex> lockGuard(m_inputQueueMutex);
	// Call callbacks of triggered handlers
	for (std::string& queuedInput : m_inputQueue)
	{
		input = std::move(queuedInput);
		if (input.length() == 1) // Can be both string/range handler
		{
			CallStringHandlers();
			CallRangeHandlers();
		}
		else // Can only be string handler
		{
			CallStringHandlers();
		}
	}
	// All callbacks of triggered handlers were called; clear trigger history
	m_inputQueue.clear();
}

void KTech::Input::Update()
{
	for (size_t i = 0; i < m_groups.size();)
	{
		// Group was set to be removed by `SetCallbacksGroupToBeRemoved`
		if (m_groups[i] == nullptr)
		{
			m_groups.erase(m_groups.begin() + i);
		}
		else
		{
			// Updates `Callback` statuses
			m_groups[i]->Update();
			i++;
		}
	}
	// Delete any `Callback` that is awaiting deletion
	for (const std::shared_ptr<Handler>& stringHandler : m_stringHandlers)
	{
		stringHandler->RemoveCallbacksSetToBeDeleted();
	}
	for (const std::shared_ptr<Handler>& rangeHandler : m_rangeHandlers)
	{
		rangeHandler->RemoveCallbacksSetToBeDeleted();
	}
}

inline void KTech::Input::CallStringHandlers()
{
	for (const std::shared_ptr<Handler>& stringHandler : m_stringHandlers) // Input matches handler's string
	{
		if (input == stringHandler->m_string) // String matches
		{
			// Call callbacks
			for (const std::shared_ptr<Callback>& callback : stringHandler->m_callbacks)
			{
				if (callback->status == Callback::Status::enabled && callback->ptr()) // Call if enabled, if returns true update render-on-demand status
				{
					m_changedThisTick = true; // Render-on-demand
				}
			}
			// Break because there shouldn't be a similar handler
			break;
		}
	}
}

void KTech::Input::CallRangeHandlers()
{
	for (const std::shared_ptr<Handler>& rangeHandler : m_rangeHandlers) // Range handlers
	{
		if (rangeHandler->m_start <= input[0] && input[0] <= rangeHandler->m_end) // Input matches handler's range
		{
			for (const std::shared_ptr<Callback>& callback: rangeHandler->m_callbacks) // Call callbacks
			{
				if (callback->status == Callback::Status::enabled && callback->ptr()) // Call if enabled, if returns true update render-on-demand status
				{
					m_changedThisTick = true; // Render-on-demand
				}
			}
		}
	}
}

void KTech::Input::Get()
{
	constexpr int maxCharacters = 7;
#ifdef _WIN32
	// Read
	TCHAR tcharBuf[7];
	memset(tcharBuf, 0, sizeof(tcharBuf));
	LPDWORD nReadCharacters = 0;
	ReadConsole(m_stdinHandle, tcharBuf, sizeof(tcharBuf) / sizeof(TCHAR), (LPDWORD)(&nReadCharacters), NULL);
	// Convert `TCHAR` string to `char` string
	std::string charBuf(static_cast<size_t>(nReadCharacters), '\0');
	for (size_t i = 0; i < static_cast<size_t>(nReadCharacters); i++)
	{
		charBuf[i] = tcharBuf[i];
	}
	// Protect `std::vector m_inputQueue`
	std::lock_guard<std::mutex> lockGuard(m_inputQueueMutex);
	// Move to input history
	m_inputQueue.push_back(std::move(charBuf));
#else
	// Read
	std::string buf(maxCharacters, '\0');
	buf.resize(read(0, buf.data(), buf.size()));
	// Protect `std::vector m_inputQueue`
	std::lock_guard<std::mutex> lockGuard(m_inputQueueMutex);
	// Move to input history
	m_inputQueue.push_back(std::move(buf));
#endif
}

void KTech::Input::Loop()
{
	while (engine.running)
	{
		// Get input
		Get();
		// Protect `std::vector m_inputQueue`
		std::lock_guard<std::mutex> lockGuard(m_inputQueueMutex);
		if (m_inputQueue[m_inputQueue.size() - 1] == quitKey)
		{
			// Quit
			engine.running = false;
			break;
		}
	}
}

KTech::Input::Input(Engine& p_engine)
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
	m_inputLoop = std::thread([this]() { this->Loop(); });
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

auto KTech::Input::CreateCallback(const std::string& p_stringKey, const std::function<bool()>& p_callback) -> std::shared_ptr<Callback>
{
	if (p_callback == nullptr) // Avoid constantly checking later whether callback is null
	{
		return nullptr;
	}
	// If a handler already exists for this input, add the callback to the calls vector
	for (const std::shared_ptr<Handler>& stringHandler : m_stringHandlers)
	{
		if (stringHandler->m_string == p_stringKey)
		{
			stringHandler->m_callbacks.push_back(std::make_shared<Callback>(p_callback, stringHandler));
			return stringHandler->m_callbacks[stringHandler->m_callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	m_stringHandlers.push_back(std::make_shared<Handler>(p_stringKey));
	// And add a callback to it
	m_stringHandlers[m_stringHandlers.size() - 1]->m_callbacks.push_back(std::make_shared<Callback>(p_callback, m_stringHandlers[m_stringHandlers.size() - 1]));
	return m_stringHandlers[m_stringHandlers.size() - 1]->m_callbacks[m_stringHandlers[m_stringHandlers.size() - 1]->m_callbacks.size() - 1]; // Last callback of last handler
}

auto KTech::Input::CrateRangedCallback(char p_start, char p_end, const std::function<bool()>& p_callback) -> std::shared_ptr<Callback>
{
	if (p_callback == nullptr) // Avoid constantly checking later whether callback is null
	{
		return nullptr;
	}
	// If a handler already exists for this input, add the callback to the calls vector
	for (const std::shared_ptr<Handler>& rangeHandler : m_rangeHandlers)
	{
		if (rangeHandler->m_start == p_start && rangeHandler->m_end == p_end)
		{
			rangeHandler->m_callbacks.push_back(std::make_shared<Callback>(p_callback, rangeHandler));
			return rangeHandler->m_callbacks[rangeHandler->m_callbacks.size() - 1]; // Last callback
		}
	}
	// Otherwise, create a new handler
	m_rangeHandlers.push_back(std::make_shared<Handler>(p_start, p_end));
	// And add a callback to it
	m_rangeHandlers[m_rangeHandlers.size() - 1]->m_callbacks.push_back(std::make_shared<Callback>(p_callback, m_rangeHandlers[m_rangeHandlers.size() - 1]));
	return m_rangeHandlers[m_rangeHandlers.size() - 1]->m_callbacks[m_rangeHandlers[m_rangeHandlers.size() - 1]->m_callbacks.size() - 1]; // Last callback
}

void KTech::Input::RegisterCallbacksGroup(CallbacksGroup* const p_callbacksGroup)
{
	m_groups.push_back(p_callbacksGroup);
}

void KTech::Input::SetCallbacksGroupToBeRemoved(CallbacksGroup* const p_callbacksGroup)
{
	for (CallbacksGroup*& group : m_groups)
	{
		if (group == p_callbacksGroup)
		{
			// Set group to `nullptr`; that will be noticed in `Input::Update()` and be removed.
			group = nullptr;
			return;
		}
	}
}