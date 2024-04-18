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

#include "widget.hpp"

#include "ui.hpp"
#include "../engine/input/callbacksgroup.hpp"
#include "../engine/engine.hpp"

KTech::Widget::Widget(KTech::Engine& p_engine, KTech::ID<KTech::UI> p_ui, KTech::Point p_pos, const std::string& p_name)
	: engine(p_engine), m_pos(p_pos), m_name(p_name), m_callbacksGroup(p_engine.input.CreateCallbacksGroup(false))
{
	engine.memory.widgets.Add(this);
	EnterUI(p_ui);
}

KTech::Widget::~Widget()
{
	m_callbacksGroup->DeleteCallbacks();
	if (engine.memory.uis.Exists(m_parentUI))
		engine.memory.uis[m_parentUI]->RemoveWidget(m_id);
	engine.memory.widgets.Remove(m_id);
}

void KTech::Widget::EnterUI(ID<UI> p_ui)
{
	if (engine.memory.uis.Exists(m_parentUI))
		engine.memory.uis[m_parentUI]->RemoveWidget(m_id);
	engine.memory.uis[p_ui]->AddWidget(m_id);
}

void KTech::Widget::Select()
{
	m_selected = true;
	m_callbacksGroup->Enable();
	RenderSelected();
};

void KTech::Widget::Deselect()
{
	m_selected = false;
	m_callbacksGroup->Disable();
	RenderUnselected();
}