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
#include <termio.h>
#include <thread>

class KTech::Input
{
public:
	struct BasicCallback;
	struct BasicHandler;
	struct RangedCallback;
	struct RangedHandler;
	class CallbacksGroup;
	
	std::string input;
	std::string quitKey = "\x03";

	// Prepares terminal, creates input loop thread
	Input(Engine* const engine);
	// Resets terminal
	~Input();

	BasicCallback* RegisterCallback(const std::string& stringKey, const std::function<void()>& callback, bool onTick = false);
	RangedCallback* RegisterRangedCallback(char key1, char key2, const std::function<void()>& callback);
	CallbacksGroup* CreateCallbacksGroup(bool enabled = true);

	bool Is(const std::string& stringKey);
	bool Is(char charKey);
	bool Bigger(char charKey);
	bool Smaller(char charKey);
	bool Between(char charKey1, char charKey2);
	
	uint8_t GetInt();

 	void CallHandlers();

private:
	Engine* const engine;

	bool inputThisTick = false;

	termios m_oldTerminalAttributes;
	std::thread m_inputLoop;
	std::vector<BasicHandler*> m_basicHandlers;
	std::vector<RangedHandler*> m_rangedHandlers;
	std::vector<CallbacksGroup*> m_groups;

	static char* Get();

	void Loop();

	friend class KTech::Output;
};