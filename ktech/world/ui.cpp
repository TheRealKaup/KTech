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

#include "ui.hpp"

#include "widget.hpp"
#include "../utility/internals.hpp"
#include "../engine/engine.hpp"

/*!
	@fn KTech::UI::UI(Engine& engine, UPoint resolution, std::string name)
	@brief Construct a `UI`.
	@param engine Parent `Engine`.
	@param resolution Image's resolution.
	@param name String name.
*/
KTech::UI::UI(Engine& p_engine, UPoint p_resolution, std::string p_name)
	: engine(p_engine), m_res(p_resolution), m_name(std::move(p_name))
{
	engine.memory.uis.Add(this);
	m_image.resize(m_res.y * m_res.x);
}

/*!
	@brief Remove all `Widget`s from itself, and itself from `Memory`.
*/
KTech::UI::~UI()
{
	RemoveAllWidgets();
	engine.memory.uis.Remove(m_id);
}

/*!
	@fn KTech::UI::AddWidget(ID<Widget>& widget)
	@brief Add a `Widget`.
	@param widget The `Widget` to add.
	@return `true` if added `Widget`. `false` if `Widget` doesn't exist in `Memory` or already contained by this `UI`.
*/
auto KTech::UI::AddWidget(const ID<Widget>& p_widget) -> bool
{
	if (!engine.memory.widgets.Exists(p_widget))
	{
		return false;
	}
	for (ID<Widget>& widget : m_widgets)
	{
		if (widget == p_widget)
		{
			return false;
		}
	}
	engine.memory.widgets[p_widget]->m_parentUI = m_id;
	m_widgets.push_back(p_widget);
	return true;
}

/*!
	@fn KTech::UI::RemoveWidget(ID<Widget>& widget)
	@brief Remove a `Widget`.
	@param widget The `Widget` to remove.
	@return `true` if removed `Widget`. `false` if `Widget` is not contained by this `UI`.
*/
auto KTech::UI::RemoveWidget(const ID<Widget>& p_widget) -> bool
{
	for (size_t i = 0; i < m_widgets.size(); i++)
	{
		if (m_widgets[i] == p_widget)
		{
			if (engine.memory.widgets.Exists(m_widgets[i]))
			{
				engine.memory.widgets[m_widgets[i]]->m_parentUI = nullID<UI>;
			}
			m_widgets.erase(m_widgets.begin() + i);
			return true;
		}
	}
	return false;
}

/*!
	@brief Remove all contained `Widget`.
	@return `true` if removed all `Widget`s. `false` there are no `Widget`s in this `UI`.
*/
auto KTech::UI::RemoveAllWidgets() -> bool
{
	if (m_widgets.empty())
	{
		return false;
	}
	for (ID<Widget>& widget : m_widgets)
	{
		if (engine.memory.widgets.Exists(widget))
		{
			engine.memory.widgets[widget]->m_parentUI = nullID<UI>;
		}
	}
	m_widgets.clear();
	return true;
}

/*!
	@fn KTech::UI::Resize(UPoint resolution)
	@brief Resize the image's resolution.
	@param resolution The new size of the image.
*/
void KTech::UI::Resize(UPoint p_resolution)
{
	m_res = p_resolution;
	m_image.resize(m_res.y * m_res.x);
}

/*!
	@brief Render all contained `Widget`s.
*/
void KTech::UI::Render()
{
	RenderBackground();

	for (ID<Widget>& widgetID : m_widgets)
	{
		KTech::Widget* widget = engine.memory.widgets[widgetID];
		if (widget != nullptr && widget->m_shown)
		{
			for (KTech::Texture& texture : widget->m_textures)
			{
				if (texture.m_active)
				{
					if (texture.m_simple)
					{
						RenderSimple(widget, texture);
					}
					else
					{
						RenderComplex(widget, texture);
					}
				}
			}
		}
	}

	RenderForeground();
}

/*!
	@fn KTech::UI::Draw

	@brief Draw the rendered image (`UI::m_image`) to `Output` so it can be printed to the terminal.

	This function redirects to `Output::Draw()` and passes the given parameters verbatim.

	@see `Output::Draw()` for parameters explanation.
*/
void KTech::UI::Draw(Point p_position, UPoint p_start, UPoint p_end, uint8_t p_alpha)
{
	return engine.output.Draw(m_image, m_res, p_position, p_start, p_end, p_alpha);
}

/*!
	@brief Virtual function called once each tick.

	You can override this in your inherited class to add whatever functionality you want.

	Called by `Memory::CallOnTicks()`.

	@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

	@see `Memory::CallOnTicks()`
	@see `Output::ShouldRenderThisTick()`
*/
auto KTech::UI::OnTick() -> bool
{
	return false;
};

inline void KTech::UI::RenderBackground()
{
	// RESET image to background
	for (CellA& cell : m_image)
	{
		cell = m_background;
	}
}

inline void KTech::UI::RenderSimple(Widget* p_widget, Texture& p_texture)
{
	// PRE-CALCULATE start and end positions for image iterator
	Point start(
		p_widget->m_pos.x + (long)p_texture.m_rPos.x, // `Widget::m_pos` is relative to `UI`, meaning it's also relative to the image
		p_widget->m_pos.y + (long)p_texture.m_rPos.y
	);
	Point end(
		start.x + static_cast<decltype(start.x)>(p_texture.m_size.x),
		start.y + (long)p_texture.m_size.y
	);

	// DELIMIT positions or return if not in range
	if (!Delimit(start, end, m_res))
	{
		return;
	}

	// DRAW character according to expected behavior
	char charToDraw = p_texture.m_value.c;
	if (DetermineCharacter(charToDraw))
	{
		for (size_t y = start.y; y < end.y; y++) // ITERATE
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				m_image[m_res.x * y + x].c = charToDraw;
			}
		}
	}

	// DRAW foreground color
	RGBA tempRGBA;
	if (BakeRGBA(tempRGBA, p_texture.m_value.f)) // Returns false if alpha is 0 and thus won't change anything
	{
		for (size_t y = start.y; y < end.y; y++) // ITERATE
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				DrawBakedToRGBA(m_image[m_res.x * y + x].f, tempRGBA);
			}
		}
	}

	// DRAW background color
	if (BakeRGBA(tempRGBA, p_texture.m_value.b)) // Returns false if alpha is 0 and thus won't change anything
	{
		for (size_t y = start.y; y < end.y; y++) // ITERATE
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				DrawBakedToRGBA(m_image[m_res.x * y + x].b, tempRGBA);
			}
		}
	}
}

inline void KTech::UI::RenderComplex(Widget* p_widget, Texture& p_texture)
{
	// PRE-CALCULATE start positions for texture and image iterators
	Point texturePos(p_widget->m_pos + p_texture.m_rPos);
	Point srcStart( // Texture start position
		texturePos.x >= 0 ? 0 : - texturePos.x, // Is texture after image? Yes: start from 0 of texture. No: start from within texture.
		texturePos.y >= 0 ? 0 : - texturePos.y
	);
	Point dstStart( // Image start position
		texturePos.x <= 0 ? 0 : texturePos.x, // Is texture before image? Yes: start from 0 of image. No: start from within image.
		texturePos.y <= 0 ? 0 : texturePos.y
	);

	// ITERATE through image and texture at the same time
	for (size_t dstY = dstStart.y, srcY = srcStart.y;
		dstY < m_res.y && srcY < p_texture.m_size.y;
		dstY++, srcY++)
	{
		for (size_t dstX = dstStart.x, srcX = srcStart.x;
			dstX < m_res.x && srcX < p_texture.m_size.x;
			dstX++, srcX++)
		{
			// DRAW character according to expected behavior
			char charToDraw = p_texture(srcX, srcY).c;
			if (DetermineCharacter(charToDraw))
			{
				m_image[m_res.x * dstY + dstX].c = charToDraw;
			}

			// DRAW foreground color
			RGBA tempRGBA;
			if (BakeRGBA(tempRGBA, p_texture(srcX, srcY).f))
			{
				DrawBakedToRGBA(m_image[m_res.x * dstY + dstX].f, tempRGBA);
			}

			// DRAW background color
			if (BakeRGBA(tempRGBA, p_texture(srcX, srcY).b))
			{
				DrawBakedToRGBA(m_image[m_res.x * dstY + dstX].b, tempRGBA);
			}
		}
	}
}

inline void KTech::UI::RenderForeground()
{
	// DRAW foreground color
	RGBA tempRGBA;
	if (BakeRGBA(tempRGBA, m_frgba))
	{
		for (size_t y = 0; y < m_res.y; y++) // ITERATE
		{
			for (size_t x = 0; x < m_res.x; x++)
			{
				DrawBakedToRGBA(m_image[m_res.x * y + x].f, tempRGBA);
			}
		}
	}

	// DRAW background color
	if (BakeRGBA(tempRGBA, m_brgba))
	{
		for (size_t y = 0; y < m_res.y; y++) // ITERATE
		{
			for (size_t x = 0; x < m_res.x; x++)
			{
				DrawBakedToRGBA(m_image[m_res.x * y + x].b, tempRGBA);
			}
		}
	}
}