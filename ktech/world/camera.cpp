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

#include "camera.hpp"

#include "texture.hpp"
#include "object.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "../utility/rgbcolors.hpp"
#include "../engine/output.hpp"
#include "../engine/engine.hpp"

KTech::Camera::Camera(Engine& p_engine, Point p_pos, UPoint p_res, const std::string& p_name)
	: engine(p_engine), m_pos(p_pos), m_res(p_res)
{
	engine.memory.cameras.Add(this);
	m_image.resize(m_res.y * m_res.x);
}

KTech::Camera::Camera(Engine& p_engine, ID<Map>& p_map, bool p_active, Point p_pos, UPoint p_res, const std::string& p_name)
	: Camera(p_engine, p_pos, p_res)
{
	EnterMap(p_map, p_active);
}

KTech::Camera::~Camera()
{
	Output::Log("<Camera[" + m_name + "]::~Camera()>", RGBColors::red);
	if (engine.memory.maps.Exists(m_parentMap))
		engine.memory.maps[m_parentMap]->RemoveCamera(m_id);
	engine.memory.cameras.Remove(m_id);
}

bool KTech::Camera::EnterMap(ID<Map>& p_map, bool p_active)
{
	if (p_map == m_parentMap || !engine.memory.maps.Exists(p_map))
		return false;
	return engine.memory.maps[p_map]->AddCamera(m_id, p_active);
}

bool KTech::Camera::LeaveMap()
{
	if (engine.memory.maps.Exists(m_parentMap))
		return engine.memory.maps[m_parentMap]->RemoveCamera(m_id);
	m_parentMap = nullID<Map>;
	return true;
}

void KTech::Camera::Resize(UPoint p_res)
{
	m_res = p_res;
	m_image.resize(m_res.y * m_res.x);
}

void KTech::Camera::Render()
{
	if (engine.memory.maps.Exists(m_parentMap))
		Render(engine.memory.maps[m_parentMap]->m_layers);
}

void KTech::Camera::Render(const std::vector<ID<Layer>>& p_layers)
{
	KTech::Point start;
	KTech::Point end;
	RGBA tempFRGBA;
	RGBA tempBRGBA;
	uint8_t tempAlpha;

	// Reset the image to background
	for (size_t i = 0; i < m_image.size(); i++)
		m_image[i] = m_background;

	for (size_t l = 0; l < p_layers.size(); l++)
	{
		KTech::Layer* layer = engine.memory.layers[p_layers[l]];
		
		if (!layer->m_visible)
			continue;

		for (size_t o = 0; o < layer->m_objects.size(); o++)
		{
			KTech::Object* obj = engine.memory.objects[layer->m_objects[o]];
			
			for (size_t t = 0; t < obj->m_textures.size(); t++)
			{
				KTech::Texture& texture = obj->m_textures[t];
				
				if (!texture.m_active)
					continue;

				// Simple texture
				if (texture.m_simple)
				{
					// Affirm texture is in range and pre-calculate starting position of rendering on image
					start.y = obj->m_pos.y + (long)texture.m_rPos.y - m_pos.y;
					if (start.y < -(long)texture.m_size.y || start.y > (long)m_res.y)
						continue;
					start.x = obj->m_pos.x + (long)texture.m_rPos.x - m_pos.x;
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
					tempAlpha = texture.m_value.f.a * layer->m_alpha / 255;
					if (tempAlpha > 0)
					{
						// Pre-calculate added color (can only store 8-bit depth)
						tempFRGBA = RGBA(
							texture.m_value.f.r * texture.m_value.f.a * layer->m_alpha / 65025,
							texture.m_value.f.g * texture.m_value.f.a * layer->m_alpha / 65025,
							texture.m_value.f.b * texture.m_value.f.a * layer->m_alpha / 65025,
							tempAlpha
						);

						for (size_t y = start.y; y < end.y; y++)
						{
							for (size_t x = start.x; x < end.x; x++)
							{
								m_image[m_res.x * y + x].f.r = tempFRGBA.r + m_image[m_res.x * y + x].f.r * (255 - tempFRGBA.a) / 255;
								m_image[m_res.x * y + x].f.g = tempFRGBA.g + m_image[m_res.x * y + x].f.g * (255 - tempFRGBA.a) / 255;
								m_image[m_res.x * y + x].f.b = tempFRGBA.b + m_image[m_res.x * y + x].f.b * (255 - tempFRGBA.a) / 255;
							}
						}
					}
					// Render background
					tempAlpha = texture.m_value.b.a * layer->m_alpha / 255;
					if (tempAlpha > 0)
					{
						tempBRGBA = RGBA(
							texture.m_value.b.r * texture.m_value.b.a * layer->m_alpha / 65025,
							texture.m_value.b.g * texture.m_value.b.a * layer->m_alpha / 65025,
							texture.m_value.b.b * texture.m_value.b.a * layer->m_alpha / 65025,
							tempAlpha
						);

						for (size_t y = start.y; y < end.y; y++)
						{
							for (size_t x = start.x; x < end.x; x++)
							{
								m_image[m_res.x * y + x].b.r = tempBRGBA.r + m_image[m_res.x * y + x].b.r * (255 - tempBRGBA.a) / 255;
								m_image[m_res.x * y + x].b.g = tempBRGBA.g + m_image[m_res.x * y + x].b.g * (255 - tempBRGBA.a) / 255;
								m_image[m_res.x * y + x].b.b = tempBRGBA.b + m_image[m_res.x * y + x].b.b * (255 - tempBRGBA.a) / 255;
							}
						}
					}
				}
				// Complex texture
				else
				{
					long y, x;
					// If the texture is before the camera, iterate with Y from point in which camera view starts, and iterate with final.y from 0.
					if (obj->m_pos.y + texture.m_rPos.y < m_pos.y)
					{
						y = m_pos.y - obj->m_pos.y - texture.m_rPos.y;
						start.y = 0;
					}
					// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
					else
					{
						y = 0;
						start.y = obj->m_pos.y + texture.m_rPos.y - m_pos.y;
					}

					// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
					for (; y < texture.m_size.y && start.y < m_res.y; y++, start.y++)
					{
						// Same goes for X
						if (obj->m_pos.x + texture.m_rPos.x < m_pos.x)
						{
							x = m_pos.x - obj->m_pos.x - texture.m_rPos.x;
							start.x = 0;
						}
						else
						{
							x = 0;
							start.x = obj->m_pos.x + texture.m_rPos.x - m_pos.x;
						}

						for (; x < texture.m_size.x && start.x < m_res.x; x++, start.x++)
						{
							if (texture(x, y).c != ' ')
								m_image[m_res.x * start.y + start.x].c = (' ' <= texture(x, y).c && texture(x, y).c <= '~') ? texture(x, y).c : ' '; // Character
							// Precalculate foreground * layer alpha (8 bit depth)
							tempAlpha = texture(x, y).f.a * layer->m_alpha / 255;
							if (tempAlpha > 0)
							{
								m_image[m_res.x * start.y + start.x].f.r = (texture(x, y).f.r * tempAlpha + m_image[m_res.x * start.y + start.x].f.r * (255 - tempAlpha)) / 255;
								m_image[m_res.x * start.y + start.x].f.g = (texture(x, y).f.g * tempAlpha + m_image[m_res.x * start.y + start.x].f.g * (255 - tempAlpha)) / 255;
								m_image[m_res.x * start.y + start.x].f.b = (texture(x, y).f.b * tempAlpha + m_image[m_res.x * start.y + start.x].f.b * (255 - tempAlpha)) / 255;
							}
							// Precalculate background * layer alpha (8 bit depth)
							tempAlpha = texture(x, y).b.a * layer->m_alpha / 255;
							if (tempAlpha > 0)
							{
								m_image[m_res.x * start.y + start.x].b.r = (texture(x, y).b.r * tempAlpha + m_image[m_res.x * start.y + start.x].b.r * (255 - tempAlpha)) / 255;
								m_image[m_res.x * start.y + start.x].b.g = (texture(x, y).b.g * tempAlpha + m_image[m_res.x * start.y + start.x].b.g * (255 - tempAlpha)) / 255;
								m_image[m_res.x * start.y + start.x].b.b = (texture(x, y).b.b * tempAlpha + m_image[m_res.x * start.y + start.x].b.b * (255 - tempAlpha)) / 255;
							}
						}
					}
				}
			}
		}
		if (layer->m_frgba.a != 0)
		{
			tempFRGBA = RGBA(layer->m_frgba.a * layer->m_frgba.r / 255, layer->m_frgba.a * layer->m_frgba.g / 255, layer->m_frgba.a * layer->m_frgba.b / 255, layer->m_frgba.a);
			for (size_t y = 0; y < m_res.y; y++)
			{
				for (size_t x = 0; x < m_res.x; x++)
				{
					m_image[m_res.x * y + x].f.r = tempFRGBA.r + (255 - layer->m_frgba.a) * m_image[m_res.x * y + x].f.r / 255;
					m_image[m_res.x * y + x].f.g = tempFRGBA.g + (255 - layer->m_frgba.a) * m_image[m_res.x * y + x].f.g / 255;
					m_image[m_res.x * y + x].f.b = tempFRGBA.b + (255 - layer->m_frgba.a) * m_image[m_res.x * y + x].f.b / 255;
				}
			}
		}
		if (layer->m_brgba.a != 0)
		{
			tempBRGBA = RGBA(layer->m_brgba.a * layer->m_brgba.r / 255, layer->m_brgba.a * layer->m_brgba.g / 255, layer->m_brgba.a * layer->m_brgba.b / 255, layer->m_brgba.a);
			for (size_t y = 0; y < m_res.y; y++)
			{
				for (size_t x = 0; x < m_res.x; x++)
				{
					m_image[m_res.x * y + x].b.r = tempBRGBA.r + (255 - layer->m_brgba.a) * m_image[m_res.x * y + x].b.r / 255;
					m_image[m_res.x * y + x].b.g = tempBRGBA.g + (255 - layer->m_brgba.a) * m_image[m_res.x * y + x].b.g / 255;
					m_image[m_res.x * y + x].b.b = tempBRGBA.b + (255 - layer->m_brgba.a) * m_image[m_res.x * y + x].b.b / 255;
				}
			}
		}
	}
}