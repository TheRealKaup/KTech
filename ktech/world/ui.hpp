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
#include "../basic/cella.hpp"
#include "../basic/rgba.hpp"
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"

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
	Engine& engine; //!< Parent `Engine`.
	const ID<UI> m_id{ID<UI>::Unique()}; //!< Personal `ID`.
	std::string m_name; //!< String name
	std::vector<ID<Widget>> m_widgets; //!< Contained `Widget`s.

	UPoint m_res; //!< Image's resolution.
	CellA m_background = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)); //!< The background to render upon.
	uint8_t m_alpha = std::numeric_limits<uint8_t>::max(); //!< Opacity for all rendered `Widget`s.
	RGBA m_frgba = RGBAColors::transparent; //!< Foreground color added after rendering `Widget`s.
	RGBA m_brgba = RGBAColors::transparent; //!< Background color added after rendering `Widget`s.
	std::vector<CellA> m_image; //!< `CellA`-based rendered image.

	UI(Engine& engine, UPoint resolution = UPoint(10, 10), std::string name = "");
	virtual ~UI();

	auto AddWidget(const ID<Widget>& widget) -> bool;
	auto RemoveWidget(const ID<Widget>& widget) -> bool;
	auto RemoveAllWidgets() -> bool;

	void Resize(UPoint resolution);

	void Render();
	void Draw(Point position = Point(0, 0), UPoint start = UPoint(0, 0), UPoint end = UPoint(0, 0), uint8_t alpha = std::numeric_limits<uint8_t>::max());
	void RenderClearDrawPrint();

protected:
	virtual auto OnTick() -> bool;

	friend class KTech::Memory;

private:
	inline void RenderBackground();
	inline void RenderSimple(Widget* widget, Texture& texture);
	inline void RenderComplex(Widget* widget, Texture& texture);
	inline void RenderForeground();
};