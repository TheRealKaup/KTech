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

#include "invocation.hpp"

/*!
	@brief Construct an `Invocation`.

	@param engine Parent `Engine`.
	@param callback Your callback function. It should return a `bool` explained in `Output::ShouldRenderThisTick()`.

	@see `Output::ShouldRenderThisTick()`
*/
KTech::Time::Invocation::Invocation(Engine& engine, const std::function<bool()>& callback)
	: engine(engine), m_callback(callback)
{
	engine.time.RegisterCallback(this);
}

/*!
	@brief Safely deregister invocation from `Time`.
*/
KTech::Time::Invocation::~Invocation()
{
	engine.time.DeregisterCallback(nullptr);
}

/*!
	@brief Invoke your callback function.

	Can be invoked once at a time. Meaning, invoking while already invoked will restart the current invocation (rather than ). For example:

	@code{.cpp}
	Engine engine;

	void Loop()
	{
		// Invoke this function after a second:
		static Invocation invocation{engine, Loop, 1, Measurement::seconds};
		invocation.Invoke();

		// Print when the game quits how many times this function was called:
		static unsigned int counter = 0;
		engine.output.outputOnQuit.push_back(std::to_string(++counter));
	}

	int main()
	{
		// Call `Loop()` twice:
		Loop();
		Loop();
	}

	// Expected `outputOnQuit`:
	// 1
	// 2
	// 3
	@endcode

	@param time Duration to wait for before calling your function.
	@param measurement The time measurement for your given `time`.

	@see `Time::Measurement`
*/
void KTech::Time::Invocation::Invoke(long time, Measurement measurement)
{
	m_duration = engine.time.TimeToMicroseconds(time, measurement);
	m_timePassed = 0;
	m_active = true;
}

/*!
	@brief Cancel the current invocation.
*/
void KTech::Time::Invocation::Cancel()
{
	m_active = false;
	m_duration = 0;
}