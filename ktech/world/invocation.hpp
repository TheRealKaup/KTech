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
	@brief Define KTech::Invocation class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../engine/time.hpp"
#include "entity.hpp"

#include <functional>

/*!
	@brief Calls a function after some time.

	This class is based on Entity to offer RAII to prevent cases where your member functions are called after their parent entity was deconstructed from memory.

	For example:

	@code{.cpp}
	class MyObject : public Object
	{
		Invocation myInvocation;

		bool InvokedMember()
		{
			// Do something...
			return true;
		}

		MyObject()
			: Object(engine), myInvocation(engine, [this]{ return InvokedMember(); })
		{
			// Invoke the member function
			myInvocation.Invoke(100, Time::Measurement::seconds);
		}
	};
	@endcode

	Imagine that within these 100 seconds, the object is destructed, so the lambda function with the `this` capture will be invalid. However, because myInvocation is a member of MyObject, it will be destructed with MyObject, which will unregister itself from memory, ensuring the invalid function is never called.
*/
class KTech::Invocation : public Entity<Invocation>
{
public:
	std::function<bool()> m_callback; //!< Function to invoke.
	bool m_active = false;			  //!< Whether timer is active or not.
	//! Time (in microseconds) that passed between start of invocation and now (if active) or invocation ended (if inactive).
	long m_timePassed = 0;
	long m_duration = 0; //!< Time (in microseconds) that should pass before function is called.

	/*!
		@brief Construct an `Invocation`.

		@param engine Parent `Engine`.
		@param callback Function to call back when the invocation is due. It should return a `bool` explained in `Output::ShouldRenderThisTick()`.
	*/
	Invocation(Engine& engine, const std::function<bool()>& callback);

	/*!
		@brief Start (or restart, if already counting down) timer to call the function.

		@param time Duration to wait for before calling the function.
		@param measurement The time measurement for your given `time`.

		@see `Time::Measurement`
	*/
	void Invoke(long time, Time::Measurement measurement);

	/*!
		@brief Cancel the current invocation.
	*/
	void Cancel();

protected:
	/*!
		@brief Progress invocation and call function if invocation is due.

		I am aware that before, when Invocation was managed by the Time engine component, this class had less overhead because it wasn't based on a virtual function, but when tackling issue #144 I noticed how well Invocation fits in as an Entity, so I had to make the change.

		@return m_callback's return value if invocation due and was called, false if invocation was not due.
	*/
	auto OnTick() -> bool final;

	friend Memory;
};
