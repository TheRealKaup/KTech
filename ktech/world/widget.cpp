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

/*!
	@fn KTech::Widget::Widget(KTech::Engine& engine, KTech::Point position, std::string name)
	@brief Construct a `Widget`.
	@param engine Parent `Engine`.
	@param position World position.
	@param name String name.
*/
KTech::Widget::Widget(KTech::Engine& p_engine, KTech::Point p_position, std::string p_name)
	: engine(p_engine), m_pos(p_position), m_name(std::move(p_name)), m_callbacksGroup(engine, false)
{
	engine.memory.widgets.Add(this);
}

/*!
	@fn KTech::Widget::Widget(KTech::Engine& engine, KTech::ID<KTech::UI> parentUI, KTech::Point position, std::string name)
	@brief Construct a `Widget` and immediately enter a `UI`.
	@param engine Parent `Engine`.
	@param parentUI The `UI` to enter.
	@param position World position.
	@param name String name.
*/
KTech::Widget::Widget(KTech::Engine& p_engine, KTech::ID<KTech::UI> p_parentUI, KTech::Point p_position, std::string p_name)
	: Widget(p_engine, p_position, std::move(p_name))
{
	EnterUI(p_parentUI);
}

/*!
	@brief Leave parent `UI` (if in one) and remove itself from `Memory`.

	Additional behavior is undocumented due to planned changes (see GitHub issue #106)
*/
KTech::Widget::~Widget()
{
	RemoveAllWidgets();
	LeaveUI();
	LeaveWidget();
	engine.memory.widgets.Remove(m_id);
}

/*!
	@brief Undocumented due to planned changes (see GitHub issue #106).
*/
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

/*!
	@brief Undocumented due to planned changes (see GitHub issue #106).
*/
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

/*!
	@brief Undocumented due to planned changes (see GitHub issue #106).
*/
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

/*!
	@brief Undocumented due to planned changes (see GitHub issue #106).
*/
auto KTech::Widget::EnterWidget(ID<Widget> p_widget) -> bool
{
	if (p_widget == m_parentWidget || p_widget == m_id || !engine.memory.widgets.Exists(p_widget))
	{
		return false;
	}
	return engine.memory.widgets[p_widget]->AddWidget(m_id);
}

/*!
	@brief Undocumented due to planned changes (see GitHub issue #106).
*/
auto KTech::Widget::LeaveWidget() -> bool
{
	if (engine.memory.widgets.Exists(m_parentWidget))
	{
		return engine.memory.widgets[m_parentWidget]->RemoveWidget(m_id);
	}
	m_parentWidget = nullID<Widget>;
	return true;
}

/*!
	@fn KTech::Widget::EnterUI(ID<UI> ui)
	@brief Enter a `UI`.
	@param ui The `UI` to enter.
	@return `true` if entered the given `UI`. `false` if the given `UI` is already the parent `UI`, doesn't exist in `Memory`, or failed to add this `Widget`.
*/
auto KTech::Widget::EnterUI(ID<UI> p_ui) -> bool
{
	if (p_ui == m_parentUI || !engine.memory.uis.Exists(p_ui))
	{
		return false;
	}
	return engine.memory.uis[p_ui]->AddWidget(m_id);
}

/*!
	@brief Leave the parent `UI`.
	@return `true` if left the `UI`. `false` if the parent `UI` failed to remove this `Widget`.
*/
auto KTech::Widget::LeaveUI() -> bool
{
	if (engine.memory.uis.Exists(m_parentUI))
	{
		return engine.memory.uis[m_parentUI]->RemoveWidget(m_id);
	}
	m_parentUI = nullID<UI>;
	return true;
}

/*!
	@brief Enable input callbacks, and call `Widget::OnSelect()`.

	Additional behavior is undocumented due to planned changes (see GitHub issue #106).

	@see `Widget::OnSelect()`.
*/
void KTech::Widget::Select()
{
	m_selected = true;
	m_callbacksGroup.Enable();
	for (ChildWidget& childWidget : m_childWidgets)
	{
		if (childWidget.oldSelected)
		{
			engine.memory.widgets[childWidget.widget]->Select();
		}
	}
	OnSelect();
};

/*!
	@brief Disable input callbacks, and call `Widget::OnDisable()`.

	Additional behavior is undocumented due to planned changes (see GitHub issue #106).

	@see `Widget::OnDisable()`.
*/
void KTech::Widget::Deselect()
{
	m_selected = false;
	m_callbacksGroup.Disable();
	for (ChildWidget& childWidget : m_childWidgets)
	{
		childWidget.oldSelected = engine.memory.widgets[childWidget.widget]->m_selected;
		engine.memory.widgets[childWidget.widget]->Deselect();
	}
	OnDeselect();
}

/*!
	@brief Let `UI` render this `Widget`, and call `Widget::OnShow()`.

	Additional behavior is undocumented due to planned changes (see GitHub issue #106).

	@see `Widget::OnShow()`.
*/
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

/*!
	@brief Don't let `UI` render this `Widget`, and call `Widget::OnHide()`.

	Additional behavior is undocumented due to planned changes (see GitHub issue #106).

	@see `Widget::OnHide()`.
*/
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

/*!
	@brief Virtual function called once each tick.

	Called by `Memory::CallOnTicks()`.

	@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

	@see `Memory::CallOnTicks()`
	@see `Output::ShouldRenderThisTick()`
*/
auto KTech::Widget::OnTick() -> bool
{
	return false;
};

/*!
	@brief Virtual function called by `Widget::Select()`.

	Usually, you would override this function in your `Widget`-inherited class to make the `Texture`s (`Widget::m_textures`) seem selected in contrast to the other, unselected, `Widget`s in your user interface.

	@see `Widget::Select()`
	@see `Button::OnSelect()` for example.
*/
void KTech::Widget::OnSelect() {}

/*!
	@brief Virtual function called by `Widget::Deselect()`.

	Usually, you would override this function in your `Widget`-inherited class to make the `Texture`s (`Widget::m_textures`) seem unselected in contrast to the one selected `Widget` in your user interface.


	@see `Widget::Deselect()`
	@see `Button::OnDeselect()` for example.
*/
void KTech::Widget::OnDeselect() {}

/*!
	@brief Virtual function called by `Widget::Show()`.

	`Widget::Show()` sets `Widget::m_shown` to `true` (which lets `UI` render this `Widget`), so, you aren't supposed to override this function to set each `Widget::m_textures->Texture::m_active` to `true`. There's no actual reason to override this function to create a complete `Widget`. However, you may still want to do something when this `Widget` is shown (for example, a visual effect).

	@see `Widget::Show()`
*/
void KTech::Widget::OnShow() {}

/*!
	@brief Virtual function called by `Widget::Hide()`.

	`Widget::Show()` sets `Widget::m_shown` to `false` (which tells `UI` to not render this `Widget`), so, you aren't supposed to override this function to set each `Widget::m_textures->Texture::m_active` to `false`. There's no actual reason to override this function to create a complete `Widget`. However, you may still want to do something when this `Widget` is hidden (for example, update a state machine).

	@see `Widget::Show()`
*/
void KTech::Widget::OnHide() {}