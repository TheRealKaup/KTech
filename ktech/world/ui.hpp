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
#include "../basic/cella.hpp"
#include "../basic/point.hpp"
#include "../basic/rgba.hpp"
#include "../basic/upoint.hpp"
#include "../utility/id.hpp"

#include <limits>
#include <string>
#include <vector>

/*!
	@brief World structure that contains and renders `Widget`s.

	This class is like a combined `Layer` and `Camera`, but for `Widget`s.

	Unlike how `Object`s are rendered, rendering `Widget`s doesn't involve multiple "layers" (`UI` is a single layer itself).

	Also, `UI`'s image is `CellA`-based in contrary to `Camera`'s `Cell`-based image. The alpha channels maintain the total opacity of the rendered image. This allows `UI`'s image to be drawn like a HUD on top of `Camera`'s image in `Output::Draw()`.
*/
class KTech::UI
{
public:
	Engine& engine;						 //!< Parent `Engine`.
	const ID<UI> m_id{ID<UI>::Unique()}; //!< Personal `ID`.
	std::string m_name;					 //!< String name
	std::vector<ID<Widget>> m_widgets;	 //!< Contained `Widget`s.

	UPoint m_res;														 //!< Image's resolution.
	CellA m_background = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)); //!< The background to render upon.
	uint8_t m_alpha = std::numeric_limits<uint8_t>::max();				 //!< Opacity for all rendered `Widget`s.
	RGBA m_frgba = RGBAColors::transparent; //!< Foreground color added after rendering `Widget`s.
	RGBA m_brgba = RGBAColors::transparent; //!< Background color added after rendering `Widget`s.
	std::vector<CellA> m_image;				//!< `CellA`-based rendered image.

	/*!
		@fn KTech::UI::UI(Engine& engine, UPoint resolution, std::string name)
		@brief Construct a `UI`.
		@param engine Parent `Engine`.
		@param resolution Image's resolution.
		@param name String name.
	*/
	UI(Engine& engine, UPoint resolution = UPoint(10, 10), std::string name = "");

	/*!
		@brief Remove all `Widget`s from itself, and itself from `Memory`.
	*/
	virtual ~UI();

	/*!
		@fn KTech::UI::AddWidget(ID<Widget>& widget)
		@brief Add a `Widget`.
		@param widget The `Widget` to add.
		@return `true` if added `Widget`. `false` if `Widget` doesn't exist in `Memory` or already contained by this `UI`.
	*/
	auto AddWidget(const ID<Widget>& widget) -> bool;

	/*!
		@fn KTech::UI::RemoveWidget(ID<Widget>& widget)
		@brief Remove a `Widget`.
		@param widget The `Widget` to remove.
		@return `true` if removed `Widget`. `false` if `Widget` is not contained by this `UI`.
	*/
	auto RemoveWidget(const ID<Widget>& widget) -> bool;

	/*!
		@brief Remove all contained `Widget`.
		@return `true` if removed all `Widget`s. `false` there are no `Widget`s in this `UI`.
	*/
	auto RemoveAllWidgets() -> bool;

	/*!
		@fn KTech::UI::Resize(UPoint resolution)
		@brief Resize the image's resolution.
		@param resolution The new size of the image.
	*/
	void Resize(UPoint resolution);

	/*!
		@brief Render all contained `Widget`s.
	*/
	void Render();

	/*!
		@fn KTech::UI::Draw

		@brief Draw the rendered image (`UI::m_image`) to `Output` so it can be printed to the terminal.

		This function redirects to `Output::Draw()` and passes the given parameters verbatim.

		@see `Output::Draw()` for parameters explanation.
	*/
	void Draw(
		Point position = Point(0, 0),
		UPoint start = UPoint(0, 0),
		UPoint end = UPoint(0, 0),
		uint8_t alpha = std::numeric_limits<uint8_t>::max()
	);

	/*!
		@brief Shortcut for `UI::Render()`, `Output::Clear()`, `UI::Draw()` and `Output::Print()`.

		This function calls the above functions with respect to "render on demand" (by checking `Output::ShouldRenderThisTick()` and `Output::ShouldPrintThisTick()`). So, you can use this function in your game loop to avoid boilerplate code while still maintaining good performance, unless you want more functionality in your graphics portion of your game loop. This function is especially convenient for testing in no-game-loop mode.

		@see `UI::Render()`
		@see `Output::Clear()`
		@see `UI::Draw()`
		@see `Output::Print()`
		@see `Engine::noGameLoopMode`
	*/
	void RenderClearDrawPrint();

protected:
	/*!
		@brief Virtual function called once each tick.

		You can override this in your inherited class to add whatever functionality you want.

		Called by `Memory::CallOnTicks()`.

		@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

		@see `Memory::CallOnTicks()`
		@see `Output::ShouldRenderThisTick()`
	*/
	virtual auto OnTick() -> bool;

	friend class KTech::Memory;

private:
	inline void RenderBackground();
	inline void RenderSimple(Widget* widget, Texture& texture);
	inline void RenderComplex(Widget* widget, Texture& texture);
	inline void RenderForeground();
};
