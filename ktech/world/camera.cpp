/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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

/*!
	@file
	@brief Define KTech::Camera members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "camera.hpp"

#include "../engine/engine.hpp"
#include "../engine/output.hpp"
#include "../utility/internals.hpp"
#include "../utility/rgbcolors.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "object.hpp"
#include "texture.hpp"

KTech::Camera::Camera(Engine& p_engine, Point p_position, UPoint p_resolution, const std::string& p_name)
	: engine(p_engine), m_pos(p_position), m_res(p_resolution)
{
	engine.memory.cameras.Add(this);
	m_image.resize(m_res.y * m_res.x);
}

KTech::Camera::Camera(
	Engine& p_engine, const ID<Map>& p_parentMap, Point p_position, UPoint p_resolution, const std::string& p_name
)
	: Camera(p_engine, p_position, p_resolution)
{
	EnterMap(p_parentMap);
}

KTech::Camera::~Camera()
{
	Output::Log("<Camera[" + m_name + "]::~Camera()>", RGBColors::red);
	LeaveMap();
	engine.memory.cameras.Remove(m_id);
}

auto KTech::Camera::EnterMap(const ID<Map>& p_map) -> bool
{
	if (p_map == m_parentMap || !engine.memory.maps.Exists(p_map))
	{
		return false;
	}
	return engine.memory.maps[p_map]->AddCamera(m_id);
}

auto KTech::Camera::LeaveMap() -> bool
{
	if (engine.memory.maps.Exists(m_parentMap))
	{
		return engine.memory.maps[m_parentMap]->RemoveCamera(m_id);
	}
	m_parentMap = ID<Map>();
	return true;
}

void KTech::Camera::Resize(UPoint p_resolution)
{
	m_res = p_resolution;
	m_image.resize(m_res.y * m_res.x);
}

void KTech::Camera::Render()
{
	if (engine.memory.maps.Exists(m_parentMap))
	{
		Render(engine.memory.maps[m_parentMap]->m_layers);
	}
}

void KTech::Camera::Render(const std::vector<ID<Layer>>& p_layers)
{
	RenderBackground();

	for (const KTech::ID<KTech::Layer>& layerID : p_layers)
	{
		KTech::Layer* layer = engine.memory.layers[layerID];
		if (layer->m_visible)
		{
			for (const KTech::ID<KTech::Object>& ObjectID : layer->m_objects)
			{
				KTech::Object* object = engine.memory.objects[ObjectID];
				for (KTech::Texture& texture : object->m_textures)
				{
					if (texture.m_active)
					{
						// Simple texture
						if (texture.m_simple)
						{
							RenderSimple(layer->m_alpha, object, texture);
						}
						// Complex texture
						else
						{
							RenderComplex(layer->m_alpha, object, texture);
						}
					}
				}
			}
		}

		RenderForeground(layer->m_frgba, layer->m_brgba);
	}
}

void KTech::Camera::Draw(Point p_position, UPoint p_start, UPoint p_end, uint8_t p_alpha)
{
	engine.output.Draw(m_image, m_res, p_position, p_start, p_end, p_alpha);
}

void KTech::Camera::RenderDrawPrint()
{
	if (engine.output.ShouldRenderThisTick())
	{
		// RENDER `Layer`s of parent `Map`
		Render();
		// DRAW the rendered image to `Output`'s image
		Draw();
		// PRINT the drawn `Output` image
		engine.output.Print();
	}
	else if (engine.output.ShouldPrintThisTick())
	{
		// PRINT the drawn `Output` image
		engine.output.Print();
	}
}

auto KTech::Camera::OnTick() -> bool
{
	return false;
};

inline void KTech::Camera::RenderBackground()
{
	// RESET image to background
	std::ranges::fill(m_image, m_background);
}

inline void KTech::Camera::RenderSimple(uint8_t p_layerAlpha, Object* p_object, Texture& p_texture)
{
	// PRE-CALCULATE start and end positions for image iterator
	Point start(p_object->m_pos + p_texture.m_rPos - m_pos);
	Point end(start.x + static_cast<long>(p_texture.m_size.x), start.y + static_cast<long>(p_texture.m_size.y));

	// DELIMIT positions or return if not in range
	if (!Delimit(start, end, m_res))
	{
		return;
	}

	// DRAW character according to expected behavior
	char charToDraw = p_texture.m_value.c;
	if (DetermineCharacter(charToDraw))
	{
		for (size_t y = start.y; y < end.y; y++) // Iterate
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				m_image[(m_res.x * y) + x].c = charToDraw;
			}
		}
	}

	// DRAW foreground color
	RGBA tempRGBA;
	// Returns false if alpha is 0 and thus won't change anything
	if (BakeRGBAWith(tempRGBA, p_texture.m_value.f, p_layerAlpha))
	{
		for (size_t y = start.y; y < end.y; y++) // ITERATE
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				DrawBakedToRGB(m_image[(m_res.x * y) + x].f, tempRGBA);
			}
		}
	}

	// DRAW background color
	// Returns false if alpha is 0 and thus won't change anything
	if (BakeRGBAWith(tempRGBA, p_texture.m_value.b, p_layerAlpha))
	{
		for (size_t y = start.y; y < end.y; y++) // ITERATE
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				DrawBakedToRGB(m_image[(m_res.x * y) + x].b, tempRGBA);
			}
		}
	}
}

inline void KTech::Camera::RenderComplex(uint8_t p_layerAlpha, Object* p_object, Texture& p_texture)
{
	// PRE-CALCULATE start positions for texture and image iterators
	Point texturePos(p_object->m_pos + p_texture.m_rPos - m_pos);
	Point srcStart( // Texture start position
		texturePos.x >= 0 ? 0 : - texturePos.x, // Is texture after image? Yes: start from 0 of texture. No: start from within texture.
		texturePos.y >= 0 ? 0 : - texturePos.y
	);
	Point dstStart( // Image start position
		texturePos.x <= 0 ? 0 : texturePos.x, // Is texture before image? Yes: start from 0 of image. No: start from within image.
		texturePos.y <= 0 ? 0 : texturePos.y
	);

	// ITERATE through image and texture at the same time
	for (size_t dstY = dstStart.y, srcY = srcStart.y; dstY < m_res.y && srcY < p_texture.m_size.y; dstY++, srcY++)
	{
		for (size_t dstX = dstStart.x, srcX = srcStart.x; dstX < m_res.x && srcX < p_texture.m_size.x; dstX++, srcX++)
		{
			// DRAW character according to expected behavior
			char charToDraw = p_texture(srcX, srcY).c;
			if (DetermineCharacter(charToDraw))
			{
				m_image[(m_res.x * dstY) + dstX].c = charToDraw;
			}

			// DRAW foreground color
			RGBA tempRGBA;
			if (BakeRGBAWith(tempRGBA, p_texture(srcX, srcY).f, p_layerAlpha))
			{
				DrawBakedToRGB(m_image[(m_res.x * dstY) + dstX].f, tempRGBA);
			}

			// DRAW background color
			if (BakeRGBAWith(tempRGBA, p_texture(srcX, srcY).b, p_layerAlpha))
			{
				DrawBakedToRGB(m_image[(m_res.x * dstY) + dstX].b, tempRGBA);
			}
		}
	}
}

inline void KTech::Camera::RenderForeground(const RGBA& p_frgba, const RGBA& p_brgba)
{
	// DRAW foreground color
	RGBA tempRGBA;
	if (BakeRGBA(tempRGBA, p_frgba))
	{
		for (Cell& cell : m_image)
		{
			DrawBakedToRGB(cell.f, tempRGBA);
		}
	}

	// DRAW background color
	if (BakeRGBA(tempRGBA, p_brgba))
	{
		for (Cell& cell : m_image)
		{
			DrawBakedToRGB(cell.b, tempRGBA);
		}
	}
}
