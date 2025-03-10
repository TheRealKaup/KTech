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

#include "camera.hpp"

#include "texture.hpp"
#include "object.hpp"
#include "layer.hpp"
#include "map.hpp"
#include "../utility/internals.hpp"
#include "../utility/rgbcolors.hpp"
#include "../engine/output.hpp"
#include "../engine/engine.hpp"

/*!
	@fn Camera::Camera(Engine &engine, Point position=Point(0, 0), UPoint resolution=UPoint(10, 10), const std::string &name="")
	@brief Prepare `Camera` for rendering.

	@param [in] engine Parent engine.
	@param [in] position World position.
	@param [in] resolution Image resolution.
	@param [in] name String name.
*/
KTech::Camera::Camera(Engine& p_engine, Point p_position, UPoint p_resolution, const std::string& p_name)
	: engine(p_engine), m_pos(p_position), m_res(p_resolution)
{
	engine.memory.cameras.Add(this);
	m_image.resize(m_res.y * m_res.x);
}

/*!
	@fn Camera::Camera(Engine& engine, const ID<Map>& parentMap, Point position=Point(0, 0), UPoint resolution=UPoint(10, 10), const std::string &name="")
	@brief Prepare `Camera` for rendering and immediately enter a `Map`.

	@param [in] engine Parent engine.
	@param [in] parentMap Parent map to immediately enter.
	@param [in] position World position.
	@param [in] resolution Image resolution.
	@param [in] name String name.

	@see `Map::m_activeCameraI`
*/
KTech::Camera::Camera(Engine& p_engine, const ID<Map>& p_parentMap, Point p_position, UPoint p_resolution, const std::string& p_name)
	: Camera(p_engine, p_position, p_resolution)
{
	EnterMap(p_parentMap);
}

//! @brief Leave the parent map (if in one) and removed itself from `Memory`.
KTech::Camera::~Camera()
{
	Output::Log("<Camera[" + m_name + "]::~Camera()>", RGBColors::red);
	LeaveMap();
	engine.memory.cameras.Remove(m_id);
}

/*!
	@fn Camera::EnterMap
	@brief Enter a parent `Map`.

	@param map Parent map to enter.

	@return True if joined given `Map`. False if already in given `Map`, given `Map` doesn't exist in `Memory`, or failed to join.

	@see `Map::m_activeCameraI`
*/
auto KTech::Camera::EnterMap(const ID<Map>& p_map) -> bool
{
	if (p_map == m_parentMap || !engine.memory.maps.Exists(p_map))
	{
		return false;
	}
	return engine.memory.maps[p_map]->AddCamera(m_id);
}

/*!
	@brief Leave the parent `Map`.

	@return True if left `Camera::m_parentMap`. False if doesn't have a parent `Map`, given `Map` doesn't exist in `Memory`, or failed to leave.

	@see `Map::m_activeCameraI`
*/
auto KTech::Camera::LeaveMap() -> bool
{
	if (engine.memory.maps.Exists(m_parentMap))
	{
		return engine.memory.maps[m_parentMap]->RemoveCamera(m_id);
	}
	m_parentMap = ID<Map>();
	return true;
}

/*!
	@fn Camera::Resize
	@brief Resize the image resolution (or "size").

	@param resolution The new resolution.
*/
void KTech::Camera::Resize(UPoint p_resolution)
{
	m_res = p_resolution;
	m_image.resize(m_res.y * m_res.x);
}

/*!
	@brief Render all `Object`s of all `Layer`s of the parent `Map`.
*/
void KTech::Camera::Render()
{
	if (engine.memory.maps.Exists(m_parentMap))
	{
		Render(engine.memory.maps[m_parentMap]->m_layers);
	}
}

/*!
	@fn Camera::Render(const std::vector<ID<Layer>>& layers)
	@brief Render all `Object`s of the given `Layer`s.

	@param layers The `Layer`s containing the `Object`s to render.
*/
void KTech::Camera::Render(const std::vector<ID<Layer>>& p_layers)
{
	RenderBackground();

	for (size_t l = 0; l < p_layers.size(); l++)
	{
		KTech::Layer* layer = engine.memory.layers[p_layers[l]];
		if (layer->m_visible)
		{
			for (size_t o = 0; o < layer->m_objects.size(); o++)
			{
				KTech::Object* object = engine.memory.objects[layer->m_objects[o]];
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

/*!
	@fn Camera::Draw
	@brief Draw the rendered image (`Camera::m_image`) to `Output` so it can be printed to the terminal.

	This function redirects to `Output::Draw()` and passes the given parameters verbatim.

	@see `Output::Draw()` for parameters explanation.
*/
void KTech::Camera::Draw(Point p_position, UPoint p_start, UPoint p_end, uint8_t p_alpha)
{
	return engine.output.Draw(m_image, m_res, p_position, p_start, p_end, p_alpha);
}

/*!
	@brief Shortcut for `Camera::Render()`, `Camera::Draw()` and `Output::Print()`.

	This function calls the above functions with respect to "render on demand" (by checking `Output::ShouldRenderThisTick()` and `Output::ShouldPrintThisTick()`). So, you can use this function in your game loop to avoid boilerplate code while still maintaining good performance, unless you want more functionality in your graphics portion of your game loop. This function is especially convenient for testing in no-game-loop mode.

	@see `Camera::Render()`
	@see `Camera::Draw()`
	@see `Output::Print()`
	@see `Engine::noGameLoopMode`
	@see [Tutorial chapter 5](https://github.com/TheRealKaup/KTech/blob/master/documentation/tutorial/tutorial.md#chapters): no-game-loop mode example that uses this function
*/
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

/*!
	@brief Virtual function called once each tick.

	You can override this in your inherited class to add whatever functionality you want.

	Called by `Memory::CallOnTicks()`.

	@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

	@see `Memory::CallOnTicks()`
	@see `Output::ShouldRenderThisTick()`
*/
auto KTech::Camera::OnTick() -> bool
{
	return false;
};

inline void KTech::Camera::RenderBackground()
{
	// RESET image to background
	for (auto & cell : m_image)
	{
		cell = m_background;
	}
}

inline void KTech::Camera::RenderSimple(uint8_t p_layerAlpha, Object* p_object, Texture& p_texture)
{
	// PRE-CALCULATE start and end positions for image iterator
	Point start(p_object->m_pos + p_texture.m_rPos - m_pos);
	Point end(
		start.x + static_cast<long>(p_texture.m_size.x),
		start.y + static_cast<long>(p_texture.m_size.y)
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
		for (size_t y = start.y; y < end.y; y++) // Iterate
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				m_image[m_res.x * y + x].c = charToDraw;
			}
		}
	}

	// DRAW foreground color
	RGBA tempRGBA;
	if (BakeRGBAWith(tempRGBA, p_texture.m_value.f, p_layerAlpha)) // Returns false if alpha is 0 and thus won't change anything
	{
		for (size_t y = start.y; y < end.y; y++) // ITERATE
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				DrawBakedToRGB(m_image[m_res.x * y + x].f, tempRGBA);
			}
		}
	}

	// DRAW background color
	if (BakeRGBAWith(tempRGBA, p_texture.m_value.b, p_layerAlpha)) // Returns false if alpha is 0 and thus won't change anything
	{
		for (size_t y = start.y; y < end.y; y++) // ITERATE
		{
			for (size_t x = start.x; x < end.x; x++)
			{
				DrawBakedToRGB(m_image[m_res.x * y + x].b, tempRGBA);
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
			if (BakeRGBAWith(tempRGBA, p_texture(srcX, srcY).f, p_layerAlpha))
			{
				DrawBakedToRGB(m_image[m_res.x * dstY + dstX].f, tempRGBA);
			}

			// DRAW background color
			if (BakeRGBAWith(tempRGBA, p_texture(srcX, srcY).b, p_layerAlpha))
			{
				DrawBakedToRGB(m_image[m_res.x * dstY + dstX].b, tempRGBA);
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
		for (size_t y = 0; y < m_res.y; y++) // ITERATE
		{
			for (size_t x = 0; x < m_res.x; x++)
			{
				DrawBakedToRGB(m_image[m_res.x * y + x].f, tempRGBA);
			}
		}
	}

	// DRAW background color
	if (BakeRGBA(tempRGBA, p_brgba))
	{
		for (size_t y = 0; y < m_res.y; y++) // ITERATE
		{
			for (size_t x = 0; x < m_res.x; x++)
			{
				DrawBakedToRGB(m_image[m_res.x * y + x].b, tempRGBA);
			}
		}
	}
}