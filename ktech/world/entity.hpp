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
	@brief Define KTech::Entity, KTech::ParentEntity, KTech::ChildEntity, and KTech::ParentChild classes
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../utility/id.hpp"

#include <string>
#include <vector>

/*!
	@brief Abstract base class for entities.

	@tparam This The entity.
*/
template <class This>
class KTech::Entity
{
public:
	Engine& m_engine;
	const ID<This> m_id{ID<This>::Unique()};
	std::string m_name;

	/*!
		@brief Add this entity to Engine::Memory.

		@param engine Parent Engine
		@param name Entity name
	*/
	Entity(Engine& engine, std::string name = "");

	/*!
		@brief Remove this entity from Engine::Memory.
	*/
	virtual ~Entity();

protected:
	/*!
		@brief Virtual function called once each tick.

		You can override this in your inherited class to add whatever functionality you want.

		Called by `Memory::CallOnTicks()`.

		@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.
	*/
	virtual auto OnTick() -> bool;

	friend class KTech::Memory;
};

/*!
	@brief An entity that has children.

	@tparam This The entity.
	@tparam Children The children of the entity, of potentially more than one type. It is variadic because entities like `Map` have more than 1 type of children.
*/
template <class This, class... Children>
class KTech::ParentEntity : public Entity<This>
{
public:
	/*!
	@brief Children, in a tuple of vectors.

	The tuple contains different types of vectors, each vector containing a single type of children. This is used, for example, by KTech::Map to hold children of type Layer and Camera.

	When writing this class, there were 2 other options:

	- Make it hold a single type of children (meaning, a single vector, without a tuple), and when a class needed multiple types of children, it would (virtual) inherit from this class for each type. This would be bad because of virtual inheritance, though the method I did choose makes it possible (or at lease easy) to do stuff like `ParentEntity::RemoveAllChildren()` because all vectors are accessible from one variable, `ParentEntity::m_children`, that can be unpacked using `std::apply`. Although, in the no-tuple method, there would be no need for `ParentEntity::RemoveAllChildren()` in the first place, because each destructor would simply clear the vector, and on destruction of the derived class, all of the base `ParentEntity` destructors would be called, and so all vectors will be cleared.
	- Composition instead of inheritance. Instead of inheriting from Entity, there would be Parent and Children classes that would be data members of entities, and would communicate with each other, as data members, over entities. Honestly I thought this option was ugly and inefficient so I dropped it.
	*/
	std::tuple<std::vector<ID<Children>>...> m_children;

	using Entity<This>::Entity;

	/*!
		@brief Remove children from this parent entity.
	*/
	~ParentEntity();

	/*!
		@brief Get children of entity.

		Convenient wrapper around `ParentEntity::m_children` that saves writing the `std::get` part.

		@tparam Child The type of the children
		@return std::vector<ID<Child>>& Reference to the vector of the children of the specified type
	*/
	template <class Child>
	auto GetChildren() -> std::vector<ID<Child>>&;

protected:
	/*!
		@brief Add a child to this entity.

		The adding logic is as follows:
		- If the added entity is already a child, return false.
		- Add the child's Entity::m_id to this entity's ParentEntity::m_children.
		- Set the child's ChildEntity::m_parent to this Entity::m_id.
		- Return true.

		@tparam Child Type of child being added.

		@param child `KTech::ID` of the child to add.

		@return true if successfully added the child.
		@return false if failed to add the child, because the added entity is already a child of this entity.
	*/
	template <class Child>
	requires (... || std::same_as<Child, Children>)
	auto Add(const ID<Child>& child) -> bool;

	/*!
		@brief Remove a child from this entity.

		The removing logic is as follows:
		- Find the child's Entity:id in this entity's ParentEntity::m_children.
		- If not found, return false.
		- Set the child's ChildEntity::m_parent to KTech::nullID.
		- Remove the child's Entity::m_id from this entity's ParentEntity::m_children.
		- Return true.

		@tparam Child Type of child being removed.

		@param child `KTech::ID` of the child to remove.

		@return true if successfully removed the child.
		@return false if failed to removed the child, because the removed entity is not a child of this entity.
	*/
	template <class Child>
	auto Remove(const ID<Child>& child) -> bool;

	/*!
		@brief Remove all children from this entity.

		The removing logic is as follows:
		- If ParentEntity::m_children is empty, return false.
		- For each entity in ParentEntity::m_children:
			- Set the child's ChildEntity::m_parent to KTech::nullID.
		- Clear this entity's ParentEntity::m_children.
		- Return true.

		@tparam Child Type of child being removed.

		@param child `KTech::ID` of the child to remove.

		@return true if successfully removed all of the children.
		@return false if no child was removed, because this entity had no children.
	*/
	auto RemoveAllChildren() -> bool;

	template <class Temp1, class Temp2> friend class ChildEntity;
	template <class Temp1, class Temp2, class... Temp3> friend class ParentChildEntity;
};

/*!
	@brief An entity that has a parent.

	@tparam This The entity.
	@tparam Parent The parent entity (can only be a single one).
*/
template <class This, class Parent>
class KTech::ChildEntity : public Entity<This>
{
public:
	/*!
		@brief The parent entity.
	*/
	ID<Parent> m_parent = nullID<Parent>;

	using Entity<This>::Entity;

	/*!
		@brief Add this child entity to the parent entity.

		Simply calls ChildEntity:Enter().

		@param engine Parent Engine
		@param parent Parent entity
		@param name Entity name
	*/
	ChildEntity(Engine& engine, const ID<Parent>& parent, std::string name = "");

	/*!
		@brief Remove this child entity from its parent entity.

		Simply calls ChildEntity:Leave().
	*/
	virtual ~ChildEntity();

protected:
	/*!
		@brief Add this child entity to a parent entity.

		Simply calls ParentEntity::Add().

		@param parent
		@return true
		@return false
	*/
	auto Enter(const ID<Parent>& parent) -> bool;

	/*!
		@brief Remove this child entity from the parent entity.

		The logic is as follows:
		- If m_parent is nullID:
			- Return false.
		- Otherwise, call ParentEntity::Remove().
		- Return ParentEntity::Remove()'s return value.

		@return true if successfully removed this entity from the parent entity.
		@return false if failed, because this entity wasn't a child of a parent entity.
	*/
	auto Leave() -> bool;

	template <class Temp1, class... Temp2> friend class ParentEntity;
};

/*!
	@brief An entity that has both children and a parent.

	For the sake of avoiding diamond inheritance and virtual base classes, ParentChildEntity simply reimplements the child-related functionality Entity adds. This is unfortunate, but it's better than dealing with initializing virtual base classes. Anyway, ChildEntity code neither is large nor meant to change frequently, so this isn't too bad.

	If you are not sure what I mean, virtual base classs are problematic because the most-derived class needs to initialize the virtual base classes, and considering Entity can't have a default constructor, users inheriting, for example, KTech::Object, will have to explicitly write in their constructor something ugly along the lines of `: Entity(engine, name), ChildEntity(engine, parent, name)`.

	@tparam This The entity.
	@tparam Parent The parent entity (can only be a single one).
*/
template <class This, class Parent, class... Children>
class KTech::ParentChildEntity : public ParentEntity<This, Children...>
{
public:
	/*!
		@copydoc ChildEntity::m_parent
	*/
	ID<Parent> m_parent = nullID<Parent>;

	using ParentEntity<This, Children...>::ParentEntity;

	/*!
		@copydoc ChildEntity::ChildEntity
	*/
	ParentChildEntity(Engine& engine, const ID<Parent>& parent, std::string name = "");

	/*!
		@copydoc ChildEntity::~ChildEntity
	*/
	virtual ~ParentChildEntity();

protected:
	/*!
		@copydoc ChildEntity::Enter
	*/
	auto Enter(const ID<Parent>& parent) -> bool;

	/*!
		@copydoc ChildEntity::Leave
	*/
	auto Leave() -> bool;

	template <class Temp1, class... Temp2> friend class ParentEntity;
};
