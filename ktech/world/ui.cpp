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

// Similar to `Camera::Render()`
void KTech::UI::Render()
{
	// Reset the image to background.
	for (size_t i = 0; i < m_image.size(); i++)
		m_image[i] = m_background;

	for (size_t w = 0; w < m_widgets.size(); w++)
	{
		KTech::Widget* widget = engine.memory.widgets[m_widgets[w]];
		
		if (!widget->m_shown)
			continue;

		for (size_t t = 0; t < widget->m_textures.size(); t++)
		{
			KTech::Texture& texture = widget->m_textures[t];
			
			if (!texture.m_active)
				continue;

			// Simple texture
			if (texture.m_simple)
			{
				// Affirm texture is in range and pre-calculate starting position of rendering on image
				Point start(0, widget->m_pos.y + (long)texture.m_rPos.y);
				if (start.y < -(long)texture.m_size.y || start.y > (long)m_res.y)
					continue;
				start.x = widget->m_pos.x + (long)texture.m_rPos.x;
				if (start.x < -(long)texture.m_size.x || start.x > (long)m_res.x)
					continue;
				// Pre-calculate ending position
				Point end(start.x + (long)texture.m_size.x, start.y + (long)texture.m_size.y);
				// Limit ending position
				if (end.x > m_res.x)
					end.x = m_res.x;
				if (end.y > m_res.y)
					end.y = m_res.y;
				// Limit starting position
				if (start.x < 0)
					start.x = 0;
				if (start.y < 0)
					start.y = 0;
				// Render character
				if (texture.m_value.c != ' ')
				{
					if (' ' <= texture.m_value.c && texture.m_value.c <= '~')
						for (size_t y = start.y; y < end.y; y++)
							for (size_t x = start.x; x < end.x; x++)
								m_image[m_res.x * y + x].c = texture.m_value.c;
					else
						for (size_t y = start.y; y < end.y; y++)
							for (size_t x = start.x; x < end.x; x++)
								m_image[m_res.x * y + x].c = ' ';
				}
				// Render foreground
				uint8_t tempAlpha = texture.m_value.f.a * m_alpha / 255;
				if (tempAlpha > 0)
				{
					// Pre-calculate added color (can only store 8-bit depth)
					RGBA tempFRGBA(
						texture.m_value.f.r * texture.m_value.f.a * m_alpha / 65025,
						texture.m_value.f.g * texture.m_value.f.a * m_alpha / 65025,
						texture.m_value.f.b * texture.m_value.f.a * m_alpha / 65025,
						tempAlpha
					);

					for (size_t y = start.y; y < end.y; y++)
					{
						for (size_t x = start.x; x < end.x; x++)
						{
							m_image[m_res.x * y + x].f.r = tempFRGBA.r + m_image[m_res.x * y + x].f.r * (255 - tempFRGBA.a) / 255;
							m_image[m_res.x * y + x].f.g = tempFRGBA.g + m_image[m_res.x * y + x].f.g * (255 - tempFRGBA.a) / 255;
							m_image[m_res.x * y + x].f.b = tempFRGBA.b + m_image[m_res.x * y + x].f.b * (255 - tempFRGBA.a) / 255;
							m_image[m_res.x * y + x].f.a += (255 - m_image[m_res.x * y + x].f.a) * tempAlpha / 255;
						}
					}
				}
				// Render background
				tempAlpha = texture.m_value.b.a * m_alpha / 255;
				if (tempAlpha > 0)
				{
					// Pre-calculate added color (can only store 8-bit depth)
					RGBA tempBRGBA(
						texture.m_value.b.r * texture.m_value.b.a * m_alpha / 65025,
						texture.m_value.b.g * texture.m_value.b.a * m_alpha / 65025,
						texture.m_value.b.b * texture.m_value.b.a * m_alpha / 65025,
						tempAlpha
					);

					for (size_t y = start.y; y < end.y; y++)
					{
						for (size_t x = start.x; x < end.x; x++)
						{
							m_image[m_res.x * y + x].b.r = tempBRGBA.r + m_image[m_res.x * y + x].b.r * (255 - tempBRGBA.a) / 255;
							m_image[m_res.x * y + x].b.g = tempBRGBA.g + m_image[m_res.x * y + x].b.g * (255 - tempBRGBA.a) / 255;
							m_image[m_res.x * y + x].b.b = tempBRGBA.b + m_image[m_res.x * y + x].b.b * (255 - tempBRGBA.a) / 255;
							m_image[m_res.x * y + x].b.a += (255 - m_image[m_res.x * y + x].b.a) * tempAlpha / 255;
						}
					}
				}
			}
			// Complex texture
			else
			{
				Point src(0, 0);
				Point dst(0, 0);
				// If the texture is before the camera, iterate with Y from point in which camera view starts, and iterate with final.y from 0.
				if (widget->m_pos.y + texture.m_rPos.y < 0)
					src.y = - widget->m_pos.y - texture.m_rPos.y;
				// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
				else
					dst.y = widget->m_pos.y + texture.m_rPos.y;

				// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
				for (; src.y < texture.m_size.y && dst.y < m_res.y; src.y++, dst.y++)
				{
					// Same goes for X
					if (widget->m_pos.x + texture.m_rPos.x < 0)
						src.x = - widget->m_pos.x - texture.m_rPos.x;
					else
						dst.x = widget->m_pos.x + texture.m_rPos.x;

					for (; src.x < texture.m_size.x && dst.x < m_res.x; src.x++, dst.x++)
					{
						if (texture(src.x, src.y).c != ' ')
							m_image[m_res.x * dst.y + dst.x].c = (' ' <= texture(src.x, src.y).c && texture(src.x, src.y).c <= '~') ? texture(src.x, src.y).c : ' '; // Character
						// Precalculate foreground * layer alpha (8 bit depth)
						uint8_t tempAlpha = texture(src.x, src.y).f.a * m_alpha / 255;
						if (tempAlpha > 0)
						{
							m_image[m_res.x * dst.y + dst.x].f.r = (texture(src.x, src.y).f.r * tempAlpha + m_image[m_res.x * dst.y + dst.x].f.r * (255 - tempAlpha)) / 255;
							m_image[m_res.x * dst.y + dst.x].f.g = (texture(src.x, src.y).f.g * tempAlpha + m_image[m_res.x * dst.y + dst.x].f.g * (255 - tempAlpha)) / 255;
							m_image[m_res.x * dst.y + dst.x].f.b = (texture(src.x, src.y).f.b * tempAlpha + m_image[m_res.x * dst.y + dst.x].f.b * (255 - tempAlpha)) / 255;
							m_image[m_res.x * dst.y + dst.x].f.a += (255 - m_image[m_res.x * src.y + src.x].f.a) * tempAlpha / 255;
						}
						// Precalculate background * layer alpha (8 bit depth)
						tempAlpha = texture(src.x, src.y).b.a * m_alpha / 255;
						if (tempAlpha > 0)
						{
							m_image[m_res.x * dst.y + dst.x].b.r = (texture(src.x, src.y).b.r * tempAlpha + m_image[m_res.x * dst.y + dst.x].b.r * (255 - tempAlpha)) / 255;
							m_image[m_res.x * dst.y + dst.x].b.g = (texture(src.x, src.y).b.g * tempAlpha + m_image[m_res.x * dst.y + dst.x].b.g * (255 - tempAlpha)) / 255;
							m_image[m_res.x * dst.y + dst.x].b.b = (texture(src.x, src.y).b.b * tempAlpha + m_image[m_res.x * dst.y + dst.x].b.b * (255 - tempAlpha)) / 255;
							m_image[m_res.x * dst.y + dst.x].b.a += (255 - m_image[m_res.x * src.y + src.x].b.a) * tempAlpha / 255;
						}
					}
				}
			}
		}
	}
	if (m_frgba.a != 0)
	{
		RGBA tempFRGBA(m_frgba.a * m_frgba.r / 255, m_frgba.a * m_frgba.g / 255, m_frgba.a * m_frgba.b / 255, m_frgba.a);
		for (size_t y = 0; y < m_res.y; y++)
		{
			for (size_t x = 0; x < m_res.x; x++)
			{
				m_image[m_res.x * y + x].f.r = tempFRGBA.r + (255 - m_frgba.a) * m_image[m_res.x * y + x].f.r / 255;
				m_image[m_res.x * y + x].f.g = tempFRGBA.g + (255 - m_frgba.a) * m_image[m_res.x * y + x].f.g / 255;
				m_image[m_res.x * y + x].f.b = tempFRGBA.b + (255 - m_frgba.a) * m_image[m_res.x * y + x].f.b / 255;
			}
		}
	}
	if (m_brgba.a != 0)
	{
		RGBA tempBRGBA(m_brgba.a * m_brgba.r / 255, m_brgba.a * m_brgba.g / 255, m_brgba.a * m_brgba.b / 255, m_brgba.a);
		for (size_t y = 0; y < m_res.y; y++)
		{
			for (size_t x = 0; x < m_res.x; x++)
			{
				m_image[m_res.x * y + x].b.r = tempBRGBA.r + (255 - m_brgba.a) * m_image[m_res.x * y + x].b.r / 255;
				m_image[m_res.x * y + x].b.g = tempBRGBA.g + (255 - m_brgba.a) * m_image[m_res.x * y + x].b.g / 255;
				m_image[m_res.x * y + x].b.b = tempBRGBA.b + (255 - m_brgba.a) * m_image[m_res.x * y + x].b.b / 255;
			}
		}
	}
}