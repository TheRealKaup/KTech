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
	@brief Define KTech::Widget members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "widget.hpp"

#include "../engine/engine.hpp"
#include "../engine/input/callbackgroup.hpp"
#include "ui.hpp"

KTech::Widget::Widget(Engine& p_engine, Point p_position, std::string p_name)
	: Entity(p_engine, std::move(p_name)), m_pos(p_position), m_callbackGroup(m_engine, false)
{
}

KTech::Widget::Widget(Engine& p_engine, const ID<UI>& p_parentUI, Point p_position, std::string p_name)
	: Widget(p_engine, p_position, std::move(p_name))
{
	EnterUI(p_parentUI);
}

KTech::Widget::~Widget()
{
	RemoveAllWidgets();
	LeaveUI();
	LeaveWidget();
}

auto KTech::Widget::AddWidget(const ID<Widget>& p_widget) -> bool
{
	if (!m_engine.memory.Exists(p_widget))
	{
		return false;
	}
	if (p_widget == m_id)
	{
		return false;
	}
	for (ChildWidget& widget : m_childWidgets)
	{
		if (p_widget == widget.widget)
		{
			return false;
		}
	}
	m_engine.memory[p_widget]->m_parentWidget = m_id;
	m_childWidgets.emplace_back(
		p_widget, m_engine.memory[p_widget]->m_selected, m_engine.memory[p_widget]->m_shown
	);
	return true;
}

auto KTech::Widget::RemoveWidget(const ID<Widget>& p_widget) -> bool
{
	for (size_t i = 0; i < m_childWidgets.size(); i++)
	{
		if (m_childWidgets[i].widget == p_widget)
		{
			if (m_engine.memory.Exists(m_childWidgets[i].widget))
			{
				m_engine.memory[m_childWidgets[i].widget]->m_parentWidget = nullID<Widget>;
			}
			m_childWidgets.erase(m_childWidgets.begin() + i);
			return true;
		}
	}
	return false;
}

auto KTech::Widget::RemoveAllWidgets() -> bool
{
	if (m_childWidgets.empty())
	{
		return false;
	}
	for (ChildWidget& childWidget : m_childWidgets)
	{
		if (m_engine.memory.Exists(childWidget.widget))
		{
			m_engine.memory[childWidget.widget]->m_parentWidget = nullID<Widget>;
		}
	}
	m_childWidgets.clear();
	return true;
}

auto KTech::Widget::EnterWidget(const ID<Widget>& p_widget) -> bool
{
	if (p_widget == m_parentWidget || p_widget == m_id || !m_engine.memory.Exists(p_widget))
	{
		return false;
	}
	return m_engine.memory[p_widget]->AddWidget(m_id);
}

auto KTech::Widget::LeaveWidget() -> bool
{
	if (m_engine.memory.Exists(m_parentWidget))
	{
		return m_engine.memory[m_parentWidget]->RemoveWidget(m_id);
	}
	m_parentWidget = nullID<Widget>;
	return true;
}

auto KTech::Widget::EnterUI(const ID<UI>& p_ui) -> bool
{
	if (p_ui == m_parentUI || !m_engine.memory.Exists(p_ui))
	{
		return false;
	}
	return m_engine.memory[p_ui]->AddWidget(m_id);
}

auto KTech::Widget::LeaveUI() -> bool
{
	if (m_engine.memory.Exists(m_parentUI))
	{
		return m_engine.memory[m_parentUI]->RemoveWidget(m_id);
	}
	m_parentUI = nullID<UI>;
	return true;
}

void KTech::Widget::Select()
{
	m_selected = true;
	m_callbackGroup.Enable();
	for (ChildWidget& childWidget : m_childWidgets)
	{
		if (childWidget.oldSelected)
		{
			m_engine.memory[childWidget.widget]->Select();
		}
	}
	OnSelect();
};

void KTech::Widget::Deselect()
{
	m_selected = false;
	m_callbackGroup.Disable();
	for (ChildWidget& childWidget : m_childWidgets)
	{
		childWidget.oldSelected = m_engine.memory[childWidget.widget]->m_selected;
		m_engine.memory[childWidget.widget]->Deselect();
	}
	OnDeselect();
}

void KTech::Widget::Show()
{
	m_shown = true;
	for (ChildWidget& childWidget : m_childWidgets)
	{
		if (childWidget.oldShown)
		{
			m_engine.memory[childWidget.widget]->Show();
		}
	}
	void OnShow();
}

void KTech::Widget::Hide()
{
	m_shown = false;
	for (ChildWidget& childWidget : m_childWidgets)
	{
		childWidget.oldShown = m_engine.memory[childWidget.widget]->m_shown;
		m_engine.memory[childWidget.widget]->Hide();
	}
	void OnHide();
}

void KTech::Widget::OnSelect()
{}

void KTech::Widget::OnDeselect()
{}

void KTech::Widget::OnShow()
{}

void KTech::Widget::OnHide()
{}
