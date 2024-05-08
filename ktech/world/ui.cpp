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
	m_image.resize(m_res.y, std::vector<CellA>(m_res.x));
}

KTech::UI::~UI()
{
	for (ID<Widget> widget : m_widgets)
		if (engine.memory.widgets.Exists(widget))
			engine.memory.widgets[widget]->m_parentUI = ID<UI>(0, 0);
	engine.memory.uis.Remove(m_id);
}

void KTech::UI::AddWidget(ID<Widget> p_widget)
{
	if (engine.memory.widgets.Exists(p_widget))
	{
		engine.memory.widgets[p_widget]->m_parentUI = m_id;
		m_widgets.push_back(p_widget);
	}
}

bool KTech::UI::RemoveWidget(ID<Widget> p_widget)
{
	for (size_t i = 0; i < m_widgets.size(); i++)
	{
		if (m_widgets[i] == p_widget)
		{
			if (engine.memory.widgets.Exists(m_widgets[i]))
				engine.memory.widgets[m_widgets[i]]->m_parentUI = ID<UI>(0, 0);
			m_widgets.erase(m_widgets.begin() + i);
			return true;
		}
	}
	return false;
}

void KTech::UI::Resize(UPoint p_res)
{
	static size_t y;
	m_res = p_res;
	m_image.resize(p_res.y);
	for (y = 0; y < p_res.y; y++)
		m_image[y].resize(p_res.x);
}

// Similar to `Camera::Render()`
void KTech::UI::Render()
{
	// Point final - the relative position between the camera and the texture (image index).
	KTech::Point start;
	KTech::Point end;
	RGBA tempFRGBA;
	RGBA tempBRGBA;
	uint8_t tempAlpha;

	// Reset the image to background.
	for (size_t y = 0; y < m_res.y; y++)
		for (size_t x = 0; x < m_res.x; x++)
			m_image[y][x] = m_background;

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
				start.y = widget->m_pos.y + (long)texture.m_rPos.y;
				if (start.y < -(long)texture.m_size.y || start.y > (long)m_res.y)
					continue;
				start.x = widget->m_pos.x + (long)texture.m_rPos.x;
				if (start.x < -(long)texture.m_size.x || start.x > (long)m_res.x)
					continue;
				// Pre-calcualte ending position
				end.y = start.y + (long)texture.m_size.y;
				if (end.y > m_res.y)
					end.y = m_res.y;
				end.x = start.x + (long)texture.m_size.x;
				if (end.x > m_res.x)
					end.x = m_res.x;
				// Correct starting position
				if (start.y < 0)
					start.y = 0;
				if (start.x < 0)
					start.x = 0;
				// Pre-calculate added color (can only store 8-bit depth)
				tempFRGBA = RGBA(
				//  8 ->                16 ->               24 ->          8.
					texture.m_value.f.r * texture.m_value.f.a * m_alpha / 65025,
					texture.m_value.f.g * texture.m_value.f.a * m_alpha / 65025,
					texture.m_value.f.b * texture.m_value.f.a * m_alpha / 65025,
				//  8 ->                16 ->          8.
					texture.m_value.f.a * m_alpha / 255
				);
				tempBRGBA = RGBA(
					texture.m_value.b.r * texture.m_value.b.a * m_alpha / 65025,
					texture.m_value.b.g * texture.m_value.b.a * m_alpha / 65025,
					texture.m_value.b.b * texture.m_value.b.a * m_alpha / 65025,
					texture.m_value.b.a * m_alpha / 255
				);
				// Render
				for (size_t y = start.y; y < end.y; y++)
				{
					for (size_t x = start.x; x < end.x; x++)
					{
						if (texture.m_value.c != ' ')
							m_image[y][x].c = texture.m_value.c;
						//                8.            8 ->              16 ->                 8.
						m_image[y][x].f.r = tempFRGBA.r + m_image[y][x].f.r * (255 - tempFRGBA.a) / 255;
						m_image[y][x].f.g = tempFRGBA.g + m_image[y][x].f.g * (255 - tempFRGBA.a) / 255;
						m_image[y][x].f.b = tempFRGBA.b + m_image[y][x].f.b * (255 - tempFRGBA.a) / 255;
						m_image[y][x].f.a += tempFRGBA.a * (255 - m_image[y][x].f.a) / 255;
						//                8.            8 ->              16 ->                 8.
						m_image[y][x].b.r = tempBRGBA.r + m_image[y][x].b.r * (255 - tempBRGBA.a) / 255;
						m_image[y][x].b.g = tempBRGBA.g + m_image[y][x].b.g * (255 - tempBRGBA.a) / 255;
						m_image[y][x].b.b = tempBRGBA.b + m_image[y][x].b.b * (255 - tempBRGBA.a) / 255;
						m_image[y][x].b.a += tempBRGBA.a * (255 - m_image[y][x].b.a) / 255;
					}
				}
			}
			// Complex texture
			else
			{
				long y, x;
				// If the texture is before the camera, iterate with Y from point in which camera view starts, and iterate with final.y from 0.
				if (widget->m_pos.y + texture.m_rPos.y < 0)
				{
					y = - widget->m_pos.y - texture.m_rPos.y;
					start.y = 0;
				}
				// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
				else
				{
					y = 0;
					start.y = widget->m_pos.y + texture.m_rPos.y;
				}

				// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
				for (; y < texture.m_t.size() && start.y < m_res.y; y++, start.y++)
				{
					// Same goes for X
					if (widget->m_pos.x + texture.m_rPos.x < 0)
					{
						x = - widget->m_pos.x - texture.m_rPos.x;
						start.x = 0;
					}
					else
					{
						x = 0;
						start.x = widget->m_pos.x + texture.m_rPos.x;
					}

					for (; x < texture.m_t[y].size() && start.x < m_res.x; x++, start.x++)
					{
						if (texture.m_t[y][x].c != ' ')
							m_image[start.y][start.x].c = texture.m_t[y][x].c; // Character
						// Precalculate foreground * layer alpha (8 bit depth)
						//          8 ->                   16 ->          8.
						tempAlpha = texture.m_t[y][x].f.a * m_alpha / 255;
						//                            (8 ->                   16.         8 ->                           16.) 16 ->          8.
						m_image[start.y][start.x].f.r = (texture.m_t[y][x].f.r * tempAlpha + m_image[start.y][start.x].f.r * (255 - tempAlpha)) / 255;
						m_image[start.y][start.x].f.g = (texture.m_t[y][x].f.g * tempAlpha + m_image[start.y][start.x].f.g * (255 - tempAlpha)) / 255;
						m_image[start.y][start.x].f.b = (texture.m_t[y][x].f.b * tempAlpha + m_image[start.y][start.x].f.b * (255 - tempAlpha)) / 255;
						m_image[start.y][start.x].f.a += tempAlpha * (255 - m_image[start.y][start.x].f.a) / 255;
						// Precalculate background * layer alpha (8 bit depth)
						//          8 ->                    16 ->                 8.
						tempAlpha = texture.m_t[y][x].b.a * m_alpha / 255;
						//                            (8 ->                   16.         8 ->                           16.) 16 ->          8.
						m_image[start.y][start.x].b.r = (texture.m_t[y][x].b.r * tempAlpha + m_image[start.y][start.x].b.r * (255 - tempAlpha)) / 255;
						m_image[start.y][start.x].b.g = (texture.m_t[y][x].b.g * tempAlpha + m_image[start.y][start.x].b.g * (255 - tempAlpha)) / 255;
						m_image[start.y][start.x].b.b = (texture.m_t[y][x].b.b * tempAlpha + m_image[start.y][start.x].b.b * (255 - tempAlpha)) / 255;
						m_image[start.y][start.x].b.a += tempAlpha * (255 - m_image[start.y][start.x].b.a) / 255;
					}
				}
			}
		}
	}
	if (m_frgba.a != 0)
	{
		tempFRGBA = RGBA(m_frgba.a * m_frgba.r / 255, m_frgba.a * m_frgba.g / 255, m_frgba.a * m_frgba.b / 255, m_frgba.a);
		for (size_t y = 0; y < m_res.y; y++)
		{
			for (size_t x = 0; x < m_res.x; x++)
			{
				m_image[y][x].f.r = tempFRGBA.r + (255 - m_frgba.a) * m_image[y][x].f.r / 255;
				m_image[y][x].f.g = tempFRGBA.g + (255 - m_frgba.a) * m_image[y][x].f.g / 255;
				m_image[y][x].f.b = tempFRGBA.b + (255 - m_frgba.a) * m_image[y][x].f.b / 255;
				m_image[y][x].f.a += tempFRGBA.a * (255 - m_image[y][x].f.a) / 255;
			}
		}
	}
	if (m_brgba.a != 0)
	{
		tempBRGBA = RGBA(m_brgba.a * m_brgba.r / 255, m_brgba.a * m_brgba.g / 255, m_brgba.a * m_brgba.b / 255, m_brgba.a);
		for (size_t y = 0; y < m_res.y; y++)
		{
			for (size_t x = 0; x < m_res.x; x++)
			{
				m_image[y][x].b.r = tempBRGBA.r + (255 - m_brgba.a) * m_image[y][x].b.r / 255;
				m_image[y][x].b.g = tempBRGBA.g + (255 - m_brgba.a) * m_image[y][x].b.g / 255;
				m_image[y][x].b.b = tempBRGBA.b + (255 - m_brgba.a) * m_image[y][x].b.b / 255;
				m_image[y][x].b.a += tempBRGBA.a * (255 - m_image[y][x].b.a) / 255;
			}
		}
	}
}