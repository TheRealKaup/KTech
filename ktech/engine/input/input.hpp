/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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
#include <memory>
#include <mutex>
#include <thread>

/*!
	@brief Engine component responsible for distributing user input.
*/
class KTech::Input
{
public:
	class CallbacksGroup;

	std::string input;
	std::string quitKey{"\x03"};

	[[nodiscard]] auto Is(const std::string& stringKey) const -> bool;
	[[nodiscard]] auto Is(char charKey) const -> bool;
	[[nodiscard]] auto Bigger(char charKey) const -> bool;
	[[nodiscard]] auto Smaller(char charKey) const -> bool;
	[[nodiscard]] auto Between(char start, char end) const -> bool;
	[[nodiscard]] auto GetInt() const -> uint8_t;

 	void CallCallbacks();

private:
	struct Handler;
	struct Callback;

	Engine& engine;
#ifdef _WIN32
	HANDLE m_stdinHandle;
	DWORD m_oldMode;
#else
	termios m_oldTerminalAttributes;
#endif
	bool m_changedThisTick = false;
	std::thread m_inputLoop;
	std::vector<std::string> m_inputQueue;
	std::mutex m_inputQueueMutex;
	// Handlers cannot be deleted; their callbacks can be deleted
	std::vector<std::shared_ptr<Handler>> m_stringHandlers;
	std::vector<std::shared_ptr<Handler>> m_rangeHandlers;
	// Groups can be deleted
	std::vector<CallbacksGroup*> m_groups;

	Input(Engine& engine);
	~Input();

	/*
		Create new `Callback` at the appropriate `Handler`.
		If appropriate `Handler` doesn't exist, first creates a new one.

		Registering `Callback`s (using this function) is indirect (the `Callback` is registered at its `Handler`, not at `Input`).
		On the other hand, removing `Callback`s is direct, because `Callbacks` directly unregisters from its `Handler`.
		This is why `Callback` has just the register function here.
	*/
	auto CreateCallback(const std::string& stringKey, const std::function<bool()>& callback) -> std::shared_ptr<Callback>;
	auto CrateRangedCallback(char start, char end, const std::function<bool()>& callback) -> std::shared_ptr<Callback>;

	/*
		Register `CallbacksGroup`.

		Unlike registering `Callback`s, registering and removing `CallbacksGroup`s is direct (the `CallbacksGroup` is registered at `Input`).
		This is why `CallbacksGroup` has a register and a remove function here.
	*/
	void RegisterCallbacksGroup(CallbacksGroup* callbacksGroup);
	/*
		Since neither `Handler`, `Callback` nor `CallbacksGroup` are expected to be removed at any point, this function prepares to remove the given pointer from `m_groups`.

		This function sets `nullptr` where the given `CallbacksGroup*` is given. `Input::Update` sees this and removes it from `m_groups`.
	*/
	void SetCallbacksGroupToBeRemoved(CallbacksGroup* callbacksGroup);

	void Update();
	void CallStringHandlers();
	void CallRangeHandlers();
	void Get();
	void Loop();

	friend class Engine;
	friend class Output;
};