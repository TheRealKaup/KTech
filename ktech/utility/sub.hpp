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
	@brief Define KTech::Sub class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../engine/engine.hpp"

#include <vector>

/*!
	@brief

	Here are some notes regarding the writing of this class. Quite a bit has gone into coming up with it.

	At first I tried extending `Entity` with `ChildEntity`, `ParentEntity` and `ParentChildEntity` base classes, which technically worked great because it completely covered adders, removeres, enterers, leavers, and even constructors and destructors, but the implementation quality was poor and had to tackle a diamond-shape inheritance (`ParentChildEntity` inherited from both `ChildEntity` and `ParentEntity`) or duplicate some code to avoid it (`ParentChildEntity` would inherit just from `ParentEntity`, and reimplement `ChildEntity`).

	Logically, inheritance, though works, isn't an appropriate generic solution to entity hierarchy. Composition (with data members) is more appropriate. Hence the class `Sub`, which offers the adding and removing logic, and of course stores the vector of the IDs of the sub entities.


	There was an entire thing regarding what `Sub` would store and how it accesses things. There were attempts to pass more template parameters, store  references to Engine or the entity itself within `Sub` and then also implement a constructor and destructor, but I settled on what you see here because I wasn't sure on what other responsibilities this class, and another potential class `Super` (which would take responsibility over the enterer and leaver as well) would do. I preferred just releasing this class already in its very simple shape, and reconsidering these things later (or hopefully hearing a suggestion made by someone else).

	The problems I had with implementing `Super` is that, first, it would require defining its member functions in separate translation units to overcome a circular dependency (of `Super` trying to members of `Sub` and vice-versa) which would require link-time optimizations for inlining (which wasn't critical, but I didn't want to bother with it). Furthermore, it adds very little in the first place if it doesn't implement a destructor, which it wouldn't because it didn't store an entity or Engine reference (which is necessary, because the destructor needs engine memory access, but doesn't accept parameters). A complicated alternative would include calculation offsets from where `Super` is stored as a data member in the entity and reaching the engine reference that way, though that would be ugly. For example, `Super::Enter` and its usage would look something like this:

	```cpp
	auto Enter(Engine& p_engine, const ID<ThisType>& p_this, const ID<SuperType>& p_super, bool (SubType::*p_adderOfSuper)(const ID<ThisType>&)) -> bool
	{
		p_engine.memory[p_super]->*p_adderOfSuper(p_this);
	}

	auto KTech::Layer::EnterMap(const ID<Map>& p_map) -> bool
	{
		return m_parentMap.Enter(m_engine, m_id, p_map, &Map::AddLayer);
	}
	```

	Which is worse than more simply writing `Layer::Enter()`:

	```cpp
	auto KTech::Layer::EnterMap(const ID<Map>& p_map) -> bool
	{
		return m_engine.memory[p_map]->AddLayer(m_id);
	}
	```

	Of course this still leaves the leaver to be implemented by the entity, which is more duplicated code:

	```
	auto KTech::Layer::LeaveMap() -> bool
	{
		if (m_engine.memory.Exists(m_parentMap))
		{
			return m_engine.memory[m_parentMap]->RemoveLayer(m_id);
		}
		return false;
	}
	```

	So the current solution isn't perfect either.

	While I don't like the amount of parameters the member functions of `Sub` takes, the compiler is likely to inline them very well within the entity's adders and removes. Having to pass all of these parameters is also still fairly acceptable in terms of boilerplate code. If you are wondering why even entities with a `Sub` still have their own adders and removers (`Layer::AddObject`, `UI::RemoveWidget`...), considering the library user could have called Layer::m_objects.Add and Remove instead; this is intentional, and there are a couple of reasons:

	- Backward compatibility.
	- Consistency: sub entities don't have a `Super` data member with enterer and leaver functions, so it would be odd to have `Layer::m_objects.Add()` and `Object::EnterLayer()`.
	- It's simply good API. It's very readable and obvious; `Layer::EnterMap`? That causes the layer to become a sublayer of the map. There is also no need to rely on the language server to properly compute whatever templates are underneath in order to autocomplete and show the user suggestions; `auto AddLayer(const ID<Layer>& layer) -> bool` is always displayed correctly. Overall, keeping this API less restricts me regarding what I implement, that is, I can make whatever data members/inheritance/templates I want, without thinking about user experience.

	To not confuse the library user, it thus made sense to set `KTech::Sub`'s member functions to private.

	The current implementation is the best option in my opinion. It differs from my previous attempts because it's simple and solves https://github.com/TheRealKaup/KTech/issues/127 as minimally as possible, except for cleanly leaving the parent entity and removing all sub entities on deconstruction. Though not the end of the world, it leaves something to be desired.

	@tparam ThisType The type of the entity with the sub entities
	@tparam SubType The type of the sub entities
*/
template <class ThisType, class SubType>
class KTech::Sub
{
public:
	/*!
		@brief Raw vector of the IDs of the sub entities.

		For safety, do not modify this vector on your own. It is exposed for advanced usage.
	*/
	std::vector<ID<SubType>> m_subs;

	/*!
		@brief Get iterator to the beginning of `Sub::m_subs`.

		@return iterator
	*/
	auto begin()
	{
		return m_subs.begin();
	}

	/*!
		@brief Get iterator to the end of `Sub::m_subs`.

		@return iterator
	*/
	auto end()
	{
		return m_subs.end();
	}

private:
	/*!
		@brief Add a sub entity.

		The adding logic is as follows:
		- If the added entity's `p_superOfSub` is already this `Entity::m_id` (`p_this`), return false.
		- Add the sub entity's ID to this `Sub::m_subs`.
		- Set the sub's `p_superOfSub` to `p_this`.
		- Return true.

		@param p_engine Engine reference.
		@param p_this The ID<ThisType> of this entity.
		@param p_sub The ID<SubType> of the entity being added.
		@param p_superOfSub The sub entity's parent ID pointer-to-member.
		@param p_leaverOfSub The sub entity's leaver pointer-to-member function.
		@return true on success, false on failure (because `p_sub` is already a sub entity here).
	*/
	auto Add(
		Engine& p_engine,
		const ID<ThisType>& p_this,
		const ID<SubType>& p_sub,
		ID<ThisType> SubType::* p_superOfSub,
		bool (SubType::*p_leaverOfSub)()
	) -> bool
	{
		if (p_engine.memory[p_sub]->*p_superOfSub == p_this)
		{
			return false;
		}
		(p_engine.memory[p_sub]->*p_leaverOfSub)();
		p_engine.memory[p_sub]->*p_superOfSub = p_this;
		m_subs.push_back(p_sub);
		return true;
	}

	/*!
		@brief Remove a sub entity.

		The removing logic is as follows:
		- Find the sub entity's ID (`p_sub`) in this `Sub::m_subs`.
		- If not found, return false.
		- Set the sub entity's parent ID (`p_superOfSub`) to `KTech::nullID`.
		- Remove the sub entity's ID from this `Sub::m_subs`.
		- Return true.

		@param p_engine Engine reference.
		@param p_sub Sub entity to remove.
		@param p_superOfSub The sub entity's parent ID pointer-to-member.
		@return true on success, false on failure (because `p_sub` is not a sub entity here).
	*/
	auto Remove(Engine& p_engine, const ID<SubType>& p_sub, ID<ThisType> SubType::* p_superOfSub) -> bool
	{
		for (size_t i = 0; i < m_subs.size(); i++)
		{
			if (m_subs[i] == p_sub)
			{
				p_engine.memory[m_subs[i]]->*p_superOfSub = nullID<ThisType>;
				m_subs.erase(m_subs.begin() + i);
				return true;
			}
		}
		return false;
	}

	/*!
		@brief Remove all sub entities.

		The removing logic is as follows:
		- If this `Sub::m_subs` is empty, return false.
		- For each entity in `Sub::m_subs`:
			- Set the sub entity's parent ID (`p_superOfSub`) to `KTech::nullID`.
		- Clear `Sub::m_subs`.
		- Return true.

		@param p_engine Engine reference.
		@param p_superOfSub The sub entity's parent ID pointer-to-member.
		@return true on success, false on failure (because there were no sub entities to remove).
	*/
	auto RemoveAll(Engine& p_engine, ID<ThisType> SubType::* p_superOfSub) -> bool
	{
		if (m_subs.empty())
		{
			return false;
		}
		for (const ID<SubType>& sub : m_subs)
		{
			p_engine.memory[sub]->*p_superOfSub = nullID<ThisType>;
		}
		m_subs.clear();
		return true;
	}

	friend ThisType;
};
