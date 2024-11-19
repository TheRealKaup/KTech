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

KTech::UI::UI(Engine& p_engine, UPoint p_res, const std::string& p_name)
	: engine(p_engine), m_res(p_res), m_name(p_name)
{
	engine.memory.uis.Add(this);
	m_image.resize(m_res.y * m_res.x);
}

KTech::UI::~UI()
{
	RemoveAllWidgets();
	engine.memory.uis.Remove(m_id);
}

bool KTech::UI::AddWidget(ID<Widget> p_widget)
{
	if (!engine.memory.widgets.Exists(p_widget))
		return false;
	for (ID<Widget>& widget : m_widgets)
		if (widget == p_widget)
			return false;
	engine.memory.widgets[p_widget]->m_parentUI = m_id;
	m_widgets.push_back(p_widget);
	return true;
}

bool KTech::UI::RemoveWidget(ID<Widget> p_widget)
{
	for (size_t i = 0; i < m_widgets.size(); i++)
	{
		if (m_widgets[i] == p_widget)
		{
			if (engine.memory.widgets.Exists(m_widgets[i]))
				engine.memory.widgets[m_widgets[i]]->m_parentUI = nullID<UI>;
			m_widgets.erase(m_widgets.begin() + i);
			return true;
		}
	}
	return false;
}

bool KTech::UI::RemoveAllWidgets()
{
	if (m_widgets.size() == 0)
		return false;
	for (size_t i = 0; i < m_widgets.size(); i++)
		if (engine.memory.widgets.Exists(m_widgets[i]))
			engine.memory.widgets[m_widgets[i]]->m_parentUI = nullID<UI>;
	m_widgets.clear();
	return true;
}

void KTech::UI::Resize(UPoint p_res)
{
	m_res = p_res;
	m_image.resize(m_res.y * m_res.x);
}

void KTech::UI::Render()
{
	RenderBackground();

	for (size_t w = 0; w < m_widgets.size(); w++)
	{
		KTech::Widget* p_widget = engine.memory.widgets[m_widgets[w]];
		if (p_widget && p_widget->m_shown)
		{
			for (KTech::Texture& texture : p_widget->m_textures)
			{
				if (texture.m_active)
				{
					if (texture.m_simple)
						RenderSimple(p_widget, texture);
					else
						RenderComplex(p_widget, texture);
				}
			}
		}
	}

	RenderForeground();
}

inline void KTech::UI::RenderBackground()
{
	// RESET image to background
	for (size_t i = 0; i < m_image.size(); i++)
		m_image[i] = m_background;
}

inline void KTech::UI::RenderSimple(Widget* p_widget, Texture& p_texture)
{
	// PRE-CALCULATE start and end positions for image iterator
	Point start(
		p_widget->m_pos.x + (long)p_texture.m_rPos.x, // `Widget::m_pos` is relative to `UI`, meaning it's also relative to the image
		p_widget->m_pos.y + (long)p_texture.m_rPos.y
	);
	Point end(
		start.x + (long)p_texture.m_size.x,
		start.y + (long)p_texture.m_size.y
	);

	// DELIMIT positions or return if not in range
	if (!Delimit(start, end, m_res))
		return;

	// DRAW character according to expected behavior
	char charToDraw = p_texture.m_value.c;
	if (DetermineCharacter(charToDraw))
		for (size_t y = start.y; y < end.y; y++) // Iterate
			for (size_t x = start.x; x < end.x; x++)
				m_image[m_res.x * y + x].c = charToDraw;

	// DRAW foreground color
	RGBA tempRGBA;
	if (BakeRGBA(tempRGBA, p_texture.m_value.f)) // Returns false if alpha is 0 and thus won't change anything
		for (size_t y = start.y; y < end.y; y++) // ITERATE
			for (size_t x = start.x; x < end.x; x++)
				DrawBakedToRGBA(m_image[m_res.x * y + x].f, tempRGBA);

	// DRAW background color
	if (BakeRGBA(tempRGBA, p_texture.m_value.b)) // Returns false if alpha is 0 and thus won't change anything
		for (size_t y = start.y; y < end.y; y++) // ITERATE
			for (size_t x = start.x; x < end.x; x++)
				DrawBakedToRGBA(m_image[m_res.x * y + x].b, tempRGBA);
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
				m_image[m_res.x * dstY + dstX].c = charToDraw;

			// DRAW foreground color
			RGBA tempRGBA;
			if (BakeRGBA(tempRGBA, p_texture(srcX, srcY).f))
				DrawBakedToRGBA(m_image[m_res.x * dstY + dstX].f, tempRGBA);

			// DRAW background color
			if (BakeRGBA(tempRGBA, p_texture(srcX, srcY).b))
				DrawBakedToRGBA(m_image[m_res.x * dstY + dstX].b, tempRGBA);
		}
	}
}

inline void KTech::UI::RenderForeground()
{
	// DRAW foreground color
	RGBA tempRGBA;
	if (BakeRGBA(tempRGBA, m_frgba))
		for (size_t y = 0; y < m_res.y; y++) // ITERATE
			for (size_t x = 0; x < m_res.x; x++)
				DrawBakedToRGBA(m_image[m_res.x * y + x].f, tempRGBA);

	// DRAW background color
	if (BakeRGBA(tempRGBA, m_brgba))
		for (size_t y = 0; y < m_res.y; y++) // ITERATE
			for (size_t x = 0; x < m_res.x; x++)
				DrawBakedToRGBA(m_image[m_res.x * y + x].b, tempRGBA);
}