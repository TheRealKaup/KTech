/*
	iteratorinvalidation, an iterator invalidation KTech test.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../../ktech/ktech.hpp"

using namespace KTech;

namespace
{
	class Object023 : public Object
	{
		using Object::Object;

	protected:
		auto OnTick() -> bool final
		{
			if (m_engine.time.ticksCounter == 0)
			{
				Output::Log("I am object " + m_name, RGBColors::green);
			}
			return false;
		}
	};

	class Object1 : public Object
	{
	public:
		Object023* m_object0;
		bool m_printedOnTick = false;

		Object1(Engine& p_engine, const ID<Layer> p_layer, std::string p_name, Object023* p_object0)
			: Object(p_engine, p_layer, {.x = 0, .y = 0}, std::move(p_name)), m_object0(p_object0)
		{}

	protected:
		auto OnTick() -> bool final
		{
			if (m_engine.time.ticksCounter == 0)
			{
				Output::Log("I am object " + m_name, RGBColors::green);
			}
			else if (m_engine.time.ticksCounter == 1)
			{
				Output::Log("Creating 20 objects", RGBColors::green);
				/*
				Insertion of objects into Memory that trigger a capacity change will of course cause an iterator invalidation. That can be fixed in CallOnTicks by not using the builtin range-based for loop.

				I have briefly considered a 2-step insertion solution to CachingRegistry, but entities are commonly created and added to other entities within the same tick, so such solution would be more complicated and ugly than simply using an index-based for loop.
				*/
				for (size_t i = 0; i < 20; i++)
				{
					new Object(m_engine);
				}
			}

			if (m_object0 != nullptr)
			{
				Output::Log("Deleting object 0", RGBColors::green);
				delete m_object0;
				m_object0 = nullptr;
				return true;
			}

			return false;
		}
	};
}

auto main() -> int
{
	Engine engine{{.x = 10, .y = 10}};

	const Map map{engine};
	const Layer layer{engine, map.m_id};

	/*
	Deleting entities before the currently-iterated entity in Memory::CallOnTicks causes the iterator to skip an entity. In this example there are 4 objects, and CallOnTicks was just called.

	Object 0's OnTick is called (v is the object CallOnTicks's iterator is pointing to):

		v
		0 1 2 3

	CallOnTicks proceeds to the next object:

		  v
		0 1 2 3

	But object 1's OnTick deletes object 0, and object 0 deletes itself from memory:

		  v
		1 2 3

	CallOnTicks doesn't know, and proceeds to the next object:

			v
		1 2 3

	Resulting in object 2's OnTick not being called. In reality, object 3 may be called twice, because the actual internal CachingRegistry vector looks like:

		 capacity
		 size   |
			v   v
	{1, 2, 3} {3}

	The solution is a 2-step removal: removing an object sets its pointer in CachingRegistry to null, and then while not iterating through the vector, erase what's null.
	*/
	auto* const object0 = new Object023{engine, layer.m_id, {.x = 0, .y = 0}, "0"};
	const Object1 object1{engine, layer.m_id, "1", object0};
	const Object023 object2{engine, {.x = 0, .y = 0}, "2"};
	const Object023 object3{engine, {.x = 0, .y = 0}, "3"};

	while (engine.running)
	{
		engine.input.CallCallbacks();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		engine.time.WaitUntilNextTick();
	}
}
