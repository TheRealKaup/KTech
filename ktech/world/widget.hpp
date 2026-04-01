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
#include "../basic/point.hpp"
#include "../engine/input/callbackgroup.hpp"
#include "../engine/input/input.hpp"
#include "../utility/id.hpp"
#include "../world/texture.hpp"

/*!
	@brief World structure that comprises `Texture`s, behaves as a user interface element, and exists within `UI`.

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
		ChildWidget(ID<Widget> widget, bool currentSelected, bool currentShown)
			: widget(widget), oldSelected(currentSelected), oldShown(currentShown)
		{}
	};

	Engine& engine;								 //!< Parent `Engine`.
	const ID<Widget> m_id{ID<Widget>::Unique()}; //!< Personal `ID`.
	std::string m_name;							 //!< String name.
	ID<UI> m_parentUI;							 //!< The `UI` containing this `Widget`.
	//! Undocumented because it's planned to change (see GitHub issue #106).
	ID<Widget> m_parentWidget = nullID<Widget>;
	//! Undocumented because it's planned to change (see GitHub issue #106).
	std::vector<ChildWidget> m_childWidgets;
	bool m_selected = false; //!< `true`: player input reaches the `Widget`. `false`: player input doesn't.
	bool m_shown = true;	 //!< `true`: will be rendered by `UI`. `false:` will be ignored by `UI`.

	Point m_pos;					 //!< World position.
	std::vector<Texture> m_textures; //!< Comprising `Texture`s.
	//! Group of all input callbacks, which are enabled and disabled in correspondence to `Widget::m_selected`.
	Input::CallbackGroup m_callbackGroup;

	/*!
		@fn KTech::Widget::Widget(Engine& engine, Point position, std::string name)
		@brief Construct a `Widget`.
		@param engine Parent `Engine`.
		@param position World position.
		@param name String name.
	*/
	Widget(Engine& engine, Point position = Point(0, 0), std::string name = "");

	/*!
		@fn KTech::Widget::Widget(Engine& engine, const ID<UI>& parentUI, Point position = Point(0, 0), std::string name = "");
		@brief Construct a `Widget` and immediately enter a `UI`.
		@param engine Parent `Engine`.
		@param parentUI The `UI` to enter.
		@param position World position.
		@param name String name.
	*/
	Widget(Engine& engine, const ID<UI>& parentUI, Point position = Point(0, 0), std::string name = "");

	/*!
		@brief Leave parent `UI` (if in one) and remove itself from `Memory`.

		Additional behavior is undocumented due to planned changes (see GitHub issue #106)
	*/
	virtual ~Widget();

	/*!
		@brief Undocumented due to planned changes (see GitHub issue #106).
	*/
	auto AddWidget(const ID<Widget>& widget) -> bool;

	/*!
		@brief Undocumented due to planned changes (see GitHub issue #106).
	*/
	auto RemoveWidget(const ID<Widget>& widget) -> bool;

	/*!
		@brief Undocumented due to planned changes (see GitHub issue #106).
	*/
	auto RemoveAllWidgets() -> bool;

	/*!
		@brief Undocumented due to planned changes (see GitHub issue #106).
	*/
	auto EnterWidget(const ID<Widget>& widget) -> bool;

	/*!
		@fn KTech::Widget::EnterUI(ID<UI> ui)
		@brief Enter a `UI`.
		@param ui The `UI` to enter.
		@return `true` if entered the given `UI`. `false` if the given `UI` is already the parent `UI`, doesn't exist in `Memory`, or failed to add this `Widget`.
	*/
	auto EnterUI(const ID<UI>& ui) -> bool;

	/*!
		@brief Undocumented due to planned changes (see GitHub issue #106).
	*/
	auto LeaveWidget() -> bool;

	/*!
		@brief Leave the parent `UI`.
		@return `true` if left the `UI`. `false` if the parent `UI` failed to remove this `Widget`.
	*/
	auto LeaveUI() -> bool;

	/*!
		@brief Enable input callbacks, and call `Widget::OnSelect()`.

		Additional behavior is undocumented due to planned changes (see GitHub issue #106).

		@see `Widget::OnSelect()`.
	*/
	void Select();

	/*!
		@brief Disable input callbacks, and call `Widget::OnDisable()`.

		Additional behavior is undocumented due to planned changes (see GitHub issue #106).

		@see `Widget::OnDisable()`.
	*/
	void Deselect();

	/*!
		@brief Let `UI` render this `Widget`, and call `Widget::OnShow()`.

		Additional behavior is undocumented due to planned changes (see GitHub issue #106).

		@see `Widget::OnShow()`.
	*/
	void Show();

	/*!
		@brief Don't let `UI` render this `Widget`, and call `Widget::OnHide()`.

		Additional behavior is undocumented due to planned changes (see GitHub issue #106).

		@see `Widget::OnHide()`.
	*/
	void Hide();

protected:
	/*!
		@brief Virtual function called once each tick.

		Called by `Memory::CallOnTicks()`.

		@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

		@see `Memory::CallOnTicks()`
		@see `Output::ShouldRenderThisTick()`
	*/
	virtual auto OnTick() -> bool;

	/*!
		@brief Virtual function called by `Widget::Select()`.

		Usually, you would override this function in your `Widget`-inherited class to make the `Texture`s (`Widget::m_textures`) seem selected in contrast to the other, unselected, `Widget`s in your user interface.

		@see `Widget::Select()`
		@see `Button::OnSelect()` for example.
	*/
	virtual void OnSelect();

	/*!
		@brief Virtual function called by `Widget::Deselect()`.

		Usually, you would override this function in your `Widget`-inherited class to make the `Texture`s (`Widget::m_textures`) seem unselected in contrast to the one selected `Widget` in your user interface.


		@see `Widget::Deselect()`
		@see `Button::OnDeselect()` for example.
	*/
	virtual void OnDeselect();

	/*!
		@brief Virtual function called by `Widget::Show()`.

		`Widget::Show()` sets `Widget::m_shown` to `true` (which lets `UI` render this `Widget`), so, you aren't supposed to override this function to set each `Widget::m_textures->Texture::m_active` to `true`. There's no actual reason to override this function to create a complete `Widget`. However, you may still want to do something when this `Widget` is shown (for example, a visual effect).

		@see `Widget::Show()`
	*/
	virtual void OnShow();

	/*!
		@brief Virtual function called by `Widget::Hide()`.

		`Widget::Show()` sets `Widget::m_shown` to `false` (which tells `UI` to not render this `Widget`), so, you aren't supposed to override this function to set each `Widget::m_textures->Texture::m_active` to `false`. There's no actual reason to override this function to create a complete `Widget`. However, you may still want to do something when this `Widget` is hidden (for example, update a state machine).

		@see `Widget::Show()`
	*/
	virtual void OnHide();

	friend class KTech::Memory;
};
