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
struct KTech::Widget
{
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
	std::vector<ChildWidget> m_childWidgets;
	bool m_selected = false;
	bool m_shown = true;

	Point m_pos;
	std::vector<Texture> m_textures = {};
	Input::CallbacksGroup* m_callbacksGroup;

	Widget(Engine& engine, ID<UI> parentUI, Point position = Point(0, 0), const std::string& name = "");
	~Widget();

	inline virtual void OnTick() {};

	inline virtual void RenderSelected () {}
	inline virtual void RenderUnselected () {}

	void AddWidget(ID<Widget> widget);
	bool RemoveWidget(ID<Widget> widget);

	void EnterUI(ID<UI> ui);
	
	void Select();
	void Deselect();
	void Show();
	void Hide();
};