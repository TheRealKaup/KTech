/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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
#include "../engine/input/callbacksgroup.hpp"

/*!
	World structure that comprises `Texture`s, behaves as a user interface element, and exists within `UI`.

	This and `Object` are the most commonly inherited-from world structures. `Widget` differs from `Object` because it can only contain `Texture`s, while `Object` can also have `Collider`s (which are useless in user interfaces). `Widget` has functionality intended for user interfaces, which is utilized by the optional widgets in the `ktech/widgets/` directory.

	@see `AboutBox`
	@see `Button`
	@see `Frame`
	@see `IntField`
	@see `StringField`
	@see `Switch`
*/
class KTech::Widget
{
public:
	//! This class is undocumented because it's planned to change (see GitHub issue #106).
	struct ChildWidget
	{
		ID<Widget> widget;
		bool oldSelected;
		bool oldShown;
		inline ChildWidget(ID<Widget> widget, bool currentSelected, bool currentShown)
			: widget(widget), oldSelected(currentSelected), oldShown(currentShown) {}
	};

	Engine& engine; //!< Parent `Engine`.
	const ID<Widget> m_id{ID<Widget>::Unique()}; //!< Personal `ID`.
	std::string m_name; //!< String name.
	ID<UI> m_parentUI; //!< The `UI` containing this `Widget`.
	ID<Widget> m_parentWidget = nullID<Widget>; //!< Undocumented because it's planned to change (see GitHub issue #106).
	std::vector<ChildWidget> m_childWidgets; //!< Undocumented because it's planned to change (see GitHub issue #106).
	bool m_selected = false; //!< `true`: player input reaches the `Widget`. `false`: player input doesn't.
	bool m_shown = true; //!< `true`: will be rendered by `UI`. `false:` will be ignored by `UI`.

	Point m_pos; //!< World position.
	std::vector<Texture> m_textures = {}; //!< Comprising `Texture`s.
	Input::CallbacksGroup m_callbacksGroup; //!< Group of all input callbacks, which are enabled and disabled in correspondence to `Widget::m_selected`.

	Widget(Engine& engine, Point position = Point(0, 0), std::string name = "");
	Widget(Engine& engine, const ID<UI>& parentUI, Point position = Point(0, 0), std::string name = "");
	virtual ~Widget();

	auto AddWidget(const ID<Widget>& widget) -> bool;
	auto RemoveWidget(const ID<Widget>& widget) -> bool;
	auto RemoveAllWidgets() -> bool;

	auto EnterWidget(const ID<Widget>& widget) -> bool;
	auto EnterUI(const ID<UI>& ui) -> bool;
	auto LeaveWidget() -> bool;
	auto LeaveUI() -> bool;

	void Select();
	void Deselect();
	void Show();
	void Hide();

protected:
	virtual auto OnTick() -> bool;
	virtual void OnSelect();
	virtual void OnDeselect();
	virtual void OnShow();
	virtual void OnHide();

	friend class KTech::Memory;
};