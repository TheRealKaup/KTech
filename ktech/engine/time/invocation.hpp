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

#include "../engine.hpp"
#include "time.hpp"

/*!
	@brief Calls a function after a given time.

	This class uses RAII to prevent cases where your member functions are called after their parent structure was deconstructed from memory.

	For example:

	@code{.cpp}
	class MyObject : Object
	{
		Invocation myInvocation;

		// A member function to invoke.
		bool InvokedMember()
		{
			// Access some memory...
			m_pos = Point(25, 25);
			m_textures.resize(10);
			return true;
		}

		MyObject()
			: Object(engine, Point(0, 0)), myInvocation(engine, [this]{ return InvokedMember(); })
		{
			// Invoke the member function:
			myInvocation.Invoke(1, Time::Measurement::seconds);
		}
	};

	int main()
	{
		//...

		// Construct a `MyObject` instance within a temporary scope:
		{
			MyObject myObject;
		}
		// Scope ends so `myObject` is destructed right away. `myObject.myInvocation` is also destructed, so even though it was just invoked, `myObject.InvokedMember()` will not be called.

		//...
	}
	@endcode
*/
struct KTech::Time::Invocation
{
	Engine& engine;					  //!< Parent `Engine`.
	std::function<bool()> m_callback; //!< Function to invoke.
	bool m_active = false;			  //!< Whether currently invoked (`true`: invoked, `false`: stationary).
	//! How much time (in microseconds) actually passed since invocation started until now (if active), or until invocation ended (if inactive).
	long m_timePassed = 0;
	//! How much time (in microseconds) should pass before your function gets called (if active), or 0 (if inactive).
	long m_duration = 0;

	/*!
		@brief Construct an `Invocation`.

		@param engine Parent `Engine`.
		@param callback Your callback function. It should return a `bool` explained in `Output::ShouldRenderThisTick()`.

		@see `Output::ShouldRenderThisTick()`
	*/
	Invocation(Engine& engine, const std::function<bool()>& callback);

	/*!
		@brief Safely deregister invocation from `Time`.
	*/
	~Invocation();

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
	void Invoke(long time, Measurement measurement);

	/*!
		@brief Cancel the current invocation.
	*/
	void Cancel();
};
