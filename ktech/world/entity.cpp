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
	@brief Define KTech::Entity, KTech::ParentEntity, KTech::ChildEntity, and KTech::ParentChild classes member functions
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION

#include "../engine/engine.hpp"
#include "entity.hpp"

#include "camera.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "object.hpp"
#include "ui.hpp"
#include "widget.hpp"

template <class This>
KTech::Entity<This>::Entity(Engine& p_engine, std::string p_name)
	: m_engine(p_engine), m_name(std::move(p_name))
{
	m_engine.memory.Add(reinterpret_cast<This*>(this));
}

template <class This>
KTech::Entity<This>::~Entity()
{
	m_engine.memory.Remove(m_id);
}

template <class This>
auto KTech::Entity<This>::OnTick() -> bool
{
	return false;
}

template <class This, class... Children>
KTech::ParentEntity<This, Children...>::~ParentEntity()
{
	RemoveAllChildren();
}

template <class This, class... Children>
template <class Child>
requires (... || std::same_as<Child, Children>)
auto KTech::ParentEntity<This, Children...>::Add(const ID<Child>& p_child) -> bool
{
	for (const ID<Child>& existingChild : GetChildren<Child>())
	{
		if (p_child == existingChild)
		{
			return false;
		}
	}
	this->m_engine.memory[p_child]->Leave();
	this->m_engine.memory[p_child]->m_parent = this->m_id;
	GetChildren<Child>().push_back(p_child);
	return true;
}

template <class This, class... Children>
template <class Child>
auto KTech::ParentEntity<This, Children...>::Remove(const ID<Child>& p_child) -> bool
{
	for (size_t i = 0; i < GetChildren<Child>().size(); i++)
	{
		if (GetChildren<Child>()[i] == p_child)
		{
			this->m_engine.memory[p_child]->m_parent = KTech::nullID<This>;
			GetChildren<Child>().erase(GetChildren<Child>().begin() + i);
			return true;
		}
	}
	return false;
}

template <class This, class... Children>
auto KTech::ParentEntity<This, Children...>::RemoveAllChildren() -> bool
{
	return std::apply(
		[this](std::vector<ID<Children>>&... p_children) -> bool {
			// Check if all are empty
			if ((... && p_children.empty()))
			{
				return false;
			}

			// For each vector, clear
			(..., [this]<class C>(std::vector<ID<C>>& p_childrenVector) -> void {
				for (const ID<C>& child : p_childrenVector)
				{
					this->m_engine.memory[child]->m_parent = KTech::nullID<This>;
				}
				p_childrenVector.clear();
			}(p_children));

			return true;
		},
		m_children
	);
}

template <class This, class... Children>
template <class Child>
auto KTech::ParentEntity<This, Children...>::GetChildren() -> std::vector<ID<Child>>&
{
	return std::get<std::vector<ID<Child>>>(m_children);
}

template <class This, class Parent>
KTech::ChildEntity<This, Parent>::ChildEntity(Engine& p_engine, const ID<Parent>& p_parent, std::string p_name)
	: Entity<This>(p_engine, std::move(p_name))
{
	Enter(p_parent);
}

template <class This, class Parent>
KTech::ChildEntity<This, Parent>::~ChildEntity()
{
	Leave();
}

template <class This, class Parent>
auto KTech::ChildEntity<This, Parent>::Enter(const ID<Parent>& p_parent) -> bool
{
	return this->m_engine.memory[p_parent]->Add(this->m_id);
}

template <class This, class Parent>
auto KTech::ChildEntity<This, Parent>::Leave() -> bool
{
	if (m_parent == KTech::nullID<Parent>)
	{
		return false;
	}
	return this->m_engine.memory[m_parent]->Remove(this->m_id);
}

template <class This, class Parent, class... Children>
KTech::ParentChildEntity<This, Parent, Children...>::ParentChildEntity(Engine& p_engine, const ID<Parent>& p_parent, std::string p_name)
	: ParentEntity<This, Children...>(p_engine, std::move(p_name))
{
	Enter(p_parent);
}

template <class This, class Parent, class... Children>
KTech::ParentChildEntity<This, Parent, Children...>::~ParentChildEntity()
{
	Leave();
}

template <class This, class Parent, class... Children>
auto KTech::ParentChildEntity<This, Parent, Children...>::Enter(const ID<Parent>& p_parent) -> bool
{
	return this->m_engine.memory[p_parent]->Add(this->m_id);
}

template <class This, class Parent, class... Children>
auto KTech::ParentChildEntity<This, Parent, Children...>::Leave() -> bool
{
	if (m_parent == KTech::nullID<Parent>)
	{
		return false;
	}
	return this->m_engine.memory[m_parent]->Remove(this->m_id);
}

template class KTech::ChildEntity<KTech::Camera, KTech::Map>;
template class KTech::ParentChildEntity<KTech::Layer, KTech::Map, KTech::Object>;
template class KTech::Entity<KTech::Map>; // Requires explicit instantiation because ParentEntity inherits Entity::Entity without implicitly instantiating it
template class KTech::ParentEntity<KTech::Map, KTech::Camera, KTech::Layer>;
template class KTech::ChildEntity<KTech::Object, KTech::Layer>;
template class KTech::Entity<KTech::UI>; // Same reason as above
template class KTech::ParentEntity<KTech::UI, KTech::Widget>;
template class KTech::ChildEntity<KTech::Widget, KTech::UI>;
