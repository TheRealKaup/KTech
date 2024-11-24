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

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../utility/id.hpp"
#include "../basic/point.hpp"
#include "../world/texture.hpp"
#include "../engine/input/input.hpp"

// Widget is now a non-optional KTech standard
class KTech::Widget
{
public:
	struct ChildWidget
	{
		ID<Widget> widget;
		bool oldSelected;
		bool oldShown;
		inline ChildWidget(ID<Widget> widget, bool currentSelected, bool currentShown)
			: widget(widget), oldSelected(currentSelected), oldShown(currentShown) {}
	};

	Engine& engine;
	ID<Widget> m_id;
	std::string m_name;
	ID<UI> m_parentUI;
	ID<Widget> m_parentWidget = nullID<Widget>;
	std::vector<ChildWidget> m_childWidgets;
	bool m_selected = false;
	bool m_shown = true;

	Point m_pos;
	std::vector<Texture> m_textures = {};
	Input::CallbacksGroup* m_callbacksGroup;

	Widget(Engine& engine, Point position = Point(0, 0), std::string name = "");
	Widget(Engine& engine, ID<UI> parentUI, Point position = Point(0, 0), std::string name = "");
	virtual ~Widget();

	auto AddWidget(ID<Widget> widget) -> bool;
	auto RemoveWidget(ID<Widget> widget) -> bool;
	auto RemoveAllWidgets() -> bool;

	auto EnterWidget(ID<Widget> widget) -> bool;
	auto EnterUI(ID<UI> ui) -> bool;
	auto LeaveWidget() -> bool;
	auto LeaveUI() -> bool;

	void Select();
	void Deselect();
	void Show();
	void Hide();

protected:
	inline virtual auto OnTick() -> bool { return false; };

	inline virtual void OnSelect () {}
	inline virtual void OnDeselect () {}
	inline virtual void OnShow () {}
	inline virtual void OnHide () {}

	friend class KTech::Memory;
};