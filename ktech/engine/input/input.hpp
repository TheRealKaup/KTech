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

#pragma once

#define KTECH_DEFINITION
#include "../../ktech.hpp"
#undef KTECH_DEFINITION

#include <functional>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#undef RGB
#else
#include <termio.h>
#endif
#include <thread>

class KTech::Input
{
public:
	struct Handler;
	struct Callback;
	class CallbacksGroup;
	
	std::string input;
	std::string quitKey{"\x03"};

	// Prepares terminal, creates input loop thread
	Input(Engine* const engine);
	// Resets terminal
	~Input();

	Callback* RegisterCallback(const std::string& stringKey, const std::function<bool()>& callback);
	Callback* RegisterRangedCallback(char key1, char key2, const std::function<bool()>& callback);
	CallbacksGroup* CreateCallbacksGroup(bool enabled = true);

	bool Is(const std::string& stringKey) const;
	bool Is(char charKey) const;
	bool Bigger(char charKey) const;
	bool Smaller(char charKey) const;
	bool Between(char charKey1, char charKey2) const;
	uint8_t GetInt() const;

 	void CallCallbacks();

private:
	Engine* const engine;

#ifdef _WIN32
	HANDLE m_stdinHandle;
	DWORD m_oldMode;
#else
	termios m_oldTerminalAttributes;
#endif

	bool changedThisTick = false;
	std::thread m_inputLoop;
	std::vector<Handler*> m_stringHandlers; // String and range handlers are split here for the sake of simplicity in `Input::CallCallbacks()` by pruning previously-existing `Handler::m_type`
	std::vector<Handler*> m_rangeHandlers;
	std::vector<CallbacksGroup*> m_groups;
	std::vector<std::string> m_triggers;
	
	void Get();

	void Loop();

	friend class Output;
};