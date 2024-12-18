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

#include "time.hpp"
#include "../engine.hpp"

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
	Engine& engine; //!< Parent `Engine`.
	std::function<bool()> m_callback; //!< Function to invoke.
	bool m_active = false; //!< Whether currently invoked (`true`: invoked, `false`: stationary).
	long m_timePassed = 0; //!< How much time (in microseconds) actually passed since invocation started until now (if active), or until invocation ended (if inactive).
	long m_duration = 0; //!< How much time (in microseconds) should pass before your function gets called (if active), or 0 (if inactive).

	Invocation(Engine& engine, const std::function<bool()>& callback);
	~Invocation();

	void Invoke(long time, Measurement measurement);
	void Cancel();
};