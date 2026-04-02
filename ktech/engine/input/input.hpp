/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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

/*!
	@file
	@brief Define KTech::Input class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
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
#include <termios.h>
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
	class CallbackGroup;

	/*!
		@var `Input::input`
		@brief Input for the last-called callback function.

		Before `Input` calls your function, it will set this string to the exact input which lead to the calling of your function. It's especially useful if you have a function that can be triggered by different inputs, like a ranged callback function (created with `CallbackGroup::RegisterRangedCallback()`): use this variable to evaluate the actual user input.
	*/
	std::string input;

	/*!
		@var `Input::quitKey`
		@brief Input that if received, breaks the input loop and sets `Engine::running` to false.

		By default, it's "\x03" (Ctrl+C), which is a common quit key among terminal applications. You may change it, but don't go Vim on your players.
	*/
	std::string quitKey{"\x03"};

	/*!
		@fn auto KTech::Input::Is(const std::string &stringKey) const -> bool
		@brief Checks if input equals given string.

		@param stringKey String to compare with `Input::input`.

		@return True if equal, otherwise false.
	*/
	[[nodiscard]] auto Is(const std::string& stringKey) const -> bool;

	/*!
		@fn auto KTech::Input::Is(char charKey) const -> bool
		@brief Checks if input equals given character.

		@param charKey Character to compare with `Input::input`.

		@return True if equal (and `Input::input` is 1 character long), otherwise false.
	*/
	[[nodiscard]] auto Is(char charKey) const -> bool;

	/*!
		@fn Input::Bigger(char charKey)
		@brief Checks if given character is bigger than input.

		@param charKey Character to compare with `Input::input`.

		@return True if bigger (and `Input::input` is 1 character long), otherwise false.
	*/
	[[nodiscard]] auto Bigger(char charKey) const -> bool;

	/*!
		@fn auto KTech::Input::Smaller(char charKey) const -> bool
		@brief Checks if given character is smaller than input.

		@param charKey Character to compare with `Input::input`.

		@return True if smaller (and `Input::input` is 1 character long), otherwise false.
	*/
	[[nodiscard]] auto Smaller(char charKey) const -> bool;

	/*!
		@fn auto KTech::Input::Between(char start, char end) const -> bool
		@brief Checks if input is between range of characters.

		@param start Start of (ASCII) character range.
		@param end End of (ASCII) character range.

		@return True if between range (and `Input::input` is 1 character long), otherwise false.
	*/
	[[nodiscard]] auto Between(char start, char end) const -> bool;

	/*!
		@brief Get the first character of input as a 1-digit number.

		@return The first character of `Input::input` subtracted by 48 (the character '0'). Can return a value that is not 1-digit-long, so unless only digit characters ('0'-'9') can call your function, you should consider confirming with `Input::Between()` and the arguments ('0', '9') that input is indeed a digit.

		@see `Input::Between()`
	*/
	[[nodiscard]] auto GetInt() const -> uint8_t;

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
	std::vector<CallbackGroup*> m_groups;

	Input(Engine& engine, bool noGameLoopMode);
	~Input();

	/*
		Create new `Callback` at the appropriate `Handler`.
		If appropriate `Handler` doesn't exist, first creates a new one.

		Registering `Callback`s (using this function) is indirect (the `Callback` is registered at its `Handler`, not at `Input`).
		On the other hand, removing `Callback`s is direct, because `Callbacks` directly unregisters from its `Handler`.
		This is why `Callback` has just the register function here.
	*/
	auto CreateCallback(const std::string& stringKey, const std::function<bool()>& callback)
		-> std::shared_ptr<Callback>;
	auto CrateRangedCallback(char start, char end, const std::function<bool()>& callback) -> std::shared_ptr<Callback>;

	/*
		Register `CallbackGroup`.

		Unlike registering `Callback`s, registering and removing `CallbackGroup`s is direct (the `CallbackGroup` is registered at `Input`).
		This is why `CallbackGroup` has a register and a remove function here.
	*/
	void RegisterCallbackGroup(CallbackGroup* callbackGroup);
	/*
		Since neither `Handler`, `Callback` nor `CallbackGroup` are expected to be removed at any point, this function prepares to remove the given pointer from `m_groups`.

		This function sets `nullptr` where the given `CallbackGroup*` is given. `Input::Update` sees this and removes it from `m_groups`.
	*/
	void SetCallbackGroupToBeRemoved(CallbackGroup* callbackGroup);

	void Update();
	void CallStringHandlers();
	void CallRangeHandlers();
	void Get();
	void Loop();

	friend class Engine;
	friend class Output;
};
