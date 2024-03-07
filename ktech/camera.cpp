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

#include "ktech.hpp"

KTech::Camera::Camera(Engine& engine, Point position, UPoint resolution, const std::string& name)
	: engine(engine), pos(position), res(resolution)
{
	engine.memory.cameras.Add(this);
	image.resize(res.y, std::vector<CellA>(res.x));
}

KTech::Camera::Camera(Engine& engine, ID<Map>& map, Point position, UPoint resolution, const std::string& name)
	: engine(engine), pos(position), res(resolution)
{
	engine.memory.cameras.Add(this);
	EnterMap(map);
	image.resize(res.y, std::vector<CellA>(res.x));
}

KTech::Camera::~Camera()
{
	IO::Log("<Camera[" + name + "]::~Camera()>", RGBColors::red);
	if (engine.memory.maps.Exists(parentMap))
		engine.memory.maps[parentMap]->RemoveCamera(id);
	engine.memory.cameras.Remove(id);
}

void KTech::Camera::EnterMap(ID<Map>& map)
{
	if (engine.memory.maps.Exists(parentMap))
		engine.memory.maps[parentMap]->RemoveCamera(id);
	engine.memory.maps[map]->AddCamera(id);
}

void KTech::Camera::Render(const std::vector<ID<Layer>>& layers)
{
	// Point final - the relative position between the camera and the texture (image index).
	static KTech::Point final;
	static RGBA tempFRGBA;
	static RGBA tempBRGBA;
	static uint8_t tempAlpha;

	// Reset the image to background.
	for (size_t y = 0; y < res.y; y++)
		for (size_t x = 0; x < res.x; x++)
			image[y][x] = background;

	for (size_t l = 0; l < layers.size(); l++)
	{
		KTech::Layer* layer = engine.memory.layers[layers[l]];

		for (size_t o = 0; o < layer->objects.size(); o++)
		{
			KTech::Object* obj = engine.memory.objects[layer->objects[o]];
			
			for (size_t t = 0; t < obj->textures.size(); t++)
			{
				KTech::Texture& texture = obj->textures[t];
				
				if (!texture.active)
					continue;

				// Simple texture
				if (texture.simple)
				{
					// Precalculate fixed values (can only store 8-bit depth)
					tempFRGBA = RGBA(
					//  8 ->                16 ->               24 ->          8.
						texture.value.f.r * texture.value.f.a * layer->alpha / 65025,
						texture.value.f.g * texture.value.f.a * layer->alpha / 65025,
						texture.value.f.b * texture.value.f.a * layer->alpha / 65025,
					//  8 ->                16 ->          8.
						texture.value.f.a * layer->alpha / 255
					);
					tempBRGBA = RGBA(
						texture.value.b.r * texture.value.b.a * layer->alpha / 65025,
						texture.value.b.g * texture.value.b.a * layer->alpha / 65025,
						texture.value.b.b * texture.value.b.a * layer->alpha / 65025,
						texture.value.b.a * layer->alpha / 255
					);

					long y, x;
					// If the texture is before the camera, iterate with Y from point in which camera view starts, and iterate with final.y from 0.
					if (obj->pos.y + texture.pos_r.y < pos.y)
					{
						y = pos.y - obj->pos.y - texture.pos_r.y;
						final.y = 0;
					}
					// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
					else
					{
						y = 0;
						final.y = obj->pos.y + texture.pos_r.y - pos.y;
					}
					// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
					for (; y < texture.size.y && final.y < res.y; y++, final.y++)
					{
						// Same goes for X
						if (obj->pos.x + texture.pos_r.x < pos.x)
						{
							x = pos.x - obj->pos.x - texture.pos_r.x;
							final.x = 0;
						}
						else
						{
							x = 0;
							final.x = obj->pos.x + texture.pos_r.x - pos.x;
						}

						for (; x < texture.size.x && final.x < res.x; x++, final.x++)
						{
							if (texture.value.c != ' ')
							{
								image[final.y][final.x].c = texture.value.c;
								//                            8.            8 ->                          16 ->       8.
								image[final.y][final.x].f.r = tempFRGBA.r + image[final.y][final.x].f.r * (255 - tempFRGBA.a) / 255;
								image[final.y][final.x].f.g = tempFRGBA.g + image[final.y][final.x].f.g * (255 - tempFRGBA.a) / 255;
								image[final.y][final.x].f.b = tempFRGBA.b + image[final.y][final.x].f.b * (255 - tempFRGBA.a) / 255;
								image[final.y][final.x].f.a += tempFRGBA.a * (255 - image[final.y][final.x].f.a) / 255;
							}
							//                            8.            8 ->                          16 ->       8.
							image[final.y][final.x].b.r = tempBRGBA.r + image[final.y][final.x].b.r * (255 - tempBRGBA.a) / 255;
							image[final.y][final.x].b.g = tempBRGBA.g + image[final.y][final.x].b.g * (255 - tempBRGBA.a) / 255;
							image[final.y][final.x].b.b = tempBRGBA.b + image[final.y][final.x].b.b * (255 - tempBRGBA.a) / 255;
							image[final.y][final.x].b.a += tempBRGBA.a * (255 - image[final.y][final.x].b.a) / 255;
						}
					}
				}
				// Complex texture
				else
				{
					long y, x;
					// If the texture is before the camera, iterate with Y from point in which camera view starts, and iterate with final.y from 0.
					if (obj->pos.y + texture.pos_r.y < pos.y)
					{
						y = pos.y - obj->pos.y - texture.pos_r.y;
						final.y = 0;
					}
					// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
					else
					{
						y = 0;
						final.y = obj->pos.y + texture.pos_r.y - pos.y;
					}

					// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
					for (; y < texture.t.size() && final.y < res.y; y++, final.y++)
					{
						// Same goes for X
						if (obj->pos.x + texture.pos_r.x < pos.x)
						{
							x = pos.x - obj->pos.x - texture.pos_r.x;
							final.x = 0;
						}
						else
						{
							x = 0;
							final.x = obj->pos.x + texture.pos_r.x - pos.x;
						}

						for (; x < texture.t[y].size() && final.x < res.x; x++, final.x++)
						{
							if (texture.t[y][x].c != ' ')
							{
								image[final.y][final.x].c = texture.t[y][x].c; // Character
								// Precalculate foreground * layer alpha (8 bit depth)
								//          8 ->                   16 ->          8.
								tempAlpha = texture.t[y][x].f.a * layer->alpha / 255;
								//                            (8 ->                   16.         8 ->                           16.) 16 ->          8.
								image[final.y][final.x].f.r = (texture.t[y][x].f.r * tempAlpha + image[final.y][final.x].f.r * (255 - tempAlpha)) / 255;
								image[final.y][final.x].f.g = (texture.t[y][x].f.g * tempAlpha + image[final.y][final.x].f.g * (255 - tempAlpha)) / 255;
								image[final.y][final.x].f.b = (texture.t[y][x].f.b * tempAlpha + image[final.y][final.x].f.b * (255 - tempAlpha)) / 255;
								image[final.y][final.x].f.a += tempAlpha * (255 - image[final.y][final.x].f.a) / 255;
							}
							// Precalculate background * layer alpha (8 bit depth)
							//          8 ->                    16 ->                 8.
							tempAlpha = texture.t[y][x].b.a * layer->alpha / 255;
							//                            (8 ->                   16.         8 ->                           16.) 16 ->          8.
							image[final.y][final.x].b.r = (texture.t[y][x].b.r * tempAlpha + image[final.y][final.x].b.r * (255 - tempAlpha)) / 255;
							image[final.y][final.x].b.g = (texture.t[y][x].b.g * tempAlpha + image[final.y][final.x].b.g * (255 - tempAlpha)) / 255;
							image[final.y][final.x].b.b = (texture.t[y][x].b.b * tempAlpha + image[final.y][final.x].b.b * (255 - tempAlpha)) / 255;
							image[final.y][final.x].b.a += tempAlpha * (255 - image[final.y][final.x].b.a) / 255;
						}
					}
				}
			}
		}
		if (layer->frgba.a != 0)
		{
			tempFRGBA = RGBA(layer->frgba.a * layer->frgba.r / 255, layer->frgba.a * layer->frgba.g / 255, layer->frgba.a * layer->frgba.b / 255, layer->frgba.a);
			for (size_t y = 0; y < res.y; y++)
			{
				for (size_t x = 0; x < res.x; x++)
				{
					image[y][x].f.r = tempFRGBA.r + (255 - layer->frgba.a) * image[y][x].f.r / 255;
					image[y][x].f.g = tempFRGBA.g + (255 - layer->frgba.a) * image[y][x].f.g / 255;
					image[y][x].f.b = tempFRGBA.b + (255 - layer->frgba.a) * image[y][x].f.b / 255;
					image[y][x].f.a += tempFRGBA.a * (255 - image[y][x].f.a) / 255;
				}
			}
		}
		if (layer->brgba.a != 0)
		{
			tempBRGBA = RGBA(layer->brgba.a * layer->brgba.r / 255, layer->brgba.a * layer->brgba.g / 255, layer->brgba.a * layer->brgba.b / 255, layer->brgba.a);
			for (size_t y = 0; y < res.y; y++)
			{
				for (size_t x = 0; x < res.x; x++)
				{
					image[y][x].b.r = tempBRGBA.r + (255 - layer->brgba.a) * image[y][x].b.r / 255;
					image[y][x].b.g = tempBRGBA.g + (255 - layer->brgba.a) * image[y][x].b.g / 255;
					image[y][x].b.b = tempBRGBA.b + (255 - layer->brgba.a) * image[y][x].b.b / 255;
					image[y][x].b.a += tempBRGBA.a * (255 - image[y][x].b.a) / 255;
				}
			}
		}
	}
}

void KTech::Camera::Render()
{
	if (engine.memory.maps.Exists(parentMap))
		Render(engine.memory.maps[parentMap]->layers);
}

void KTech::Camera::Resize(UPoint newRes)
{
	static size_t y;
	res = newRes;
	image.resize(newRes.y);
	for (y = 0; y < newRes.y; y++)
		image[y].resize(newRes.x);
}