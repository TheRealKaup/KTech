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
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../basic/point.hpp"

#include <termio.h>
#include <vector>
#include <string>
#include <thread>
#include <functional>

struct KTech::IO
{
	Engine* engine;

	termios oldTerminalAttributes;
	
	winsize terminalSize;
	std::vector<std::vector<Cell>> image;
	std::string stringImage;

	std::thread t_inputLoop;

	// Prepares terminal, creates input loop thread
	IO(KTech::UPoint imageSize, Engine* engine);
	// Resets terminal
	~IO();

	void PrintStartupNotice(const std::string& title, const std::string& years, const std::string author, const std::string programName);

	// Draw, usually the image of a camera, to the IO
	void Draw(const std::vector<std::vector<CellA>>& image, Point pos = Point(0, 0), uint16_t left = 0, uint16_t top = 0, uint16_t right = 0, uint16_t bottom = 0, uint8_t alpha = 255);

	void Print();

	static void Log(const std::string& text, RGB color);

	static char* Get();

	struct Callback
	{
		bool enabled = true;
		std::function<void()> ptr;
		inline Callback(const std::function<void()>& callback) : ptr(callback) {}
	};

	struct BasicHandler
	{
		struct BasicCallback : Callback
		{
			BasicHandler* parentHandler;
			bool onTick;
			inline BasicCallback(const std::function<void()>& callback, BasicHandler* parentHandler, bool onTick) : Callback(callback), parentHandler(parentHandler), onTick(onTick) {}
			~BasicCallback();
		};
		std::vector<BasicCallback*> callbacks; // Callbacks are stored as pointers because the vector changes its size, and CallbackGroups need a consistent pointer to the their callbacks.
		std::string input;
		uint8_t timesPressed = 0;
		inline BasicHandler(const std::string& input) : input(input) {};
		void RemoveCallback(BasicCallback*);
	};

	struct RangedHandler
	{
		struct RangedCallback : Callback
		{
			RangedHandler* parentHandler;
			inline RangedCallback(const std::function<void()>& callback, RangedHandler* parentHandler) : Callback(callback), parentHandler(parentHandler) {}
			~RangedCallback();
		};
		std::vector<RangedCallback*> callbacks; // Callbacks are stored as pointers because the vector changes its size, and CallbackGroups need a consistent pointer to the their callbacks.
		char key1, key2;
		inline RangedHandler(char key1, char key2) : key1(key1), key2(key2) {};
		void RemoveCallback(RangedCallback*);
	};

	struct CallbacksGroup
	{
		enum class Status : uint8_t
		{
			disabled,
			enabled,
			removeDisabled, // Remove and then return status to disabled
			removeEnabled, // Remove and then return status to enabled
		};
		std::vector<BasicHandler::BasicCallback*> basicCallbacks;
		std::vector<RangedHandler::RangedCallback*> rangedCallbacks;
		Status status;
		bool synced = true;
		inline CallbacksGroup(bool enabled = true) : status(enabled ? Status::enabled : Status::disabled) { }
		inline void AddCallback(BasicHandler::BasicCallback* callback) { basicCallbacks.push_back(callback); callback->enabled = false; }
		inline void AddCallback(RangedHandler::RangedCallback* callback) { rangedCallbacks.push_back(callback); callback->enabled = false; }
		inline void Enable() { status = Status::enabled; synced = false; }
		inline void Disable() { status = Status::disabled; synced = false; }
		// Remove
		void DeleteCallbacks();
	};

	std::vector<BasicHandler*> basicHandlers; // Never deleted, no need to store in pointers; handleded by indices
	std::vector<RangedHandler*> rangedHandlers; // Never deleted, no need to store in pointers; handleded by indices
	std::vector<CallbacksGroup*> groups; // Usually experiences deletions; handleded by pointers

	BasicHandler::BasicCallback* RegisterCallback(const std::string& stringKey, const std::function<void()>& callback, bool onTick = false);
	RangedHandler::RangedCallback* RegisterRangedCallback(char key1, char key2, const std::function<void()>& callback);
	CallbacksGroup* CreateCallbacksGroup(bool enabled = true);

	std::string input = std::string(7, '\0');
	std::string quitKey = "\x03";

	void Call();

	bool Is(const std::string& stringKey);
	bool Is(char charKey);
	bool Bigger(char charKey);
	bool Smaller(char charKey);
	bool Between(char charKey1, char charKey2);
	uint8_t GetInt();

	void Loop();
};