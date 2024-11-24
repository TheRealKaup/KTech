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

KTech::Widget::Widget(KTech::Engine& p_engine, KTech::Point p_position, std::string p_name)
	: engine(p_engine), m_pos(p_position), m_name(std::move(p_name)), m_callbacksGroup(p_engine.input.CreateCallbacksGroup(false))
{
	engine.memory.widgets.Add(this);
}

KTech::Widget::Widget(KTech::Engine& p_engine, KTech::ID<KTech::UI> p_parentUI, KTech::Point p_position, std::string p_name)
	: Widget(p_engine, p_position, std::move(p_name))
{
	EnterUI(p_parentUI);
}

KTech::Widget::~Widget()
{
	RemoveAllWidgets();
	LeaveUI();
	LeaveWidget();
	engine.memory.widgets.Remove(m_id);
	m_callbacksGroup->DeleteCallbacks();
}

auto KTech::Widget::AddWidget(ID<Widget> p_widget) -> bool
{
	if (!engine.memory.widgets.Exists(p_widget))
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
	engine.memory.widgets[p_widget]->m_parentWidget = m_id;
	m_childWidgets.emplace_back(p_widget, engine.memory.widgets[p_widget]->m_selected, engine.memory.widgets[p_widget]->m_shown);
	return true;
}

auto KTech::Widget::RemoveWidget(ID<Widget> p_widget) -> bool
{
	for (size_t i = 0; i < m_childWidgets.size(); i++)
	{
		if (m_childWidgets[i].widget == p_widget)
		{
			if (engine.memory.widgets.Exists(m_childWidgets[i].widget))
			{
				engine.memory.widgets[m_childWidgets[i].widget]->m_parentWidget = nullID<Widget>;
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
		if (engine.memory.widgets.Exists(childWidget.widget))
		{
			engine.memory.widgets[childWidget.widget]->m_parentWidget = nullID<Widget>;
		}
	}
	m_childWidgets.clear();
	return true;
}

auto KTech::Widget::EnterWidget(ID<Widget> p_widget) -> bool
{
	if (p_widget == m_parentWidget || p_widget == m_id || !engine.memory.widgets.Exists(p_widget))
	{
		return false;
	}
	return engine.memory.widgets[p_widget]->AddWidget(m_id);
}

auto KTech::Widget::LeaveWidget() -> bool
{
	if (engine.memory.widgets.Exists(m_parentWidget))
	{
		return engine.memory.widgets[m_parentWidget]->RemoveWidget(m_id);
	}
	m_parentWidget = nullID<Widget>;
	return true;
}

auto KTech::Widget::EnterUI(ID<UI> p_ui) -> bool
{
	if (p_ui == m_parentUI || !engine.memory.uis.Exists(p_ui))
	{
		return false;
	}
	return engine.memory.uis[p_ui]->AddWidget(m_id);
}

auto KTech::Widget::LeaveUI() -> bool
{
	if (engine.memory.uis.Exists(m_parentUI))
	{
		return engine.memory.uis[m_parentUI]->RemoveWidget(m_id);
	}
	m_parentUI = nullID<UI>;
	return true;
}

void KTech::Widget::Select()
{
	m_selected = true;
	m_callbacksGroup->Enable();
	for (ChildWidget& childWidget : m_childWidgets)
	{
		if (childWidget.oldSelected)
		{
			engine.memory.widgets[childWidget.widget]->Select();
		}
	}
	OnSelect();
};

void KTech::Widget::Deselect()
{
	m_selected = false;
	m_callbacksGroup->Disable();
	for (ChildWidget& childWidget : m_childWidgets)
	{
		childWidget.oldSelected = engine.memory.widgets[childWidget.widget]->m_selected;
		engine.memory.widgets[childWidget.widget]->Deselect();
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
			engine.memory.widgets[childWidget.widget]->Show();
		}
	}
	void OnShow();
}

void KTech::Widget::Hide()
{
	m_shown = false;
	for (ChildWidget& childWidget : m_childWidgets)
	{
		childWidget.oldShown = engine.memory.widgets[childWidget.widget]->m_shown;
		engine.memory.widgets[childWidget.widget]->Hide();
	}
	void OnHide();
}