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

KTech::UI::UI(Engine& engine, UPoint resolution, const std::string& name)
	: engine(engine), res(resolution), name(name)
{
	engine.memory.uis.Add(this);
	image.resize(res.y, std::vector<CellA>(res.x));
}

KTech::UI::~UI()
{
	for (ID<Widget> widget : widgets)
		if (engine.memory.widgets.Exists(widget))
			engine.memory.widgets[widget]->parentUI = ID<UI>(0, 0);
	engine.memory.uis.Remove(id);
}

void KTech::UI::AddWidget(ID<Widget> widget)
{
	if (engine.memory.widgets.Exists(widget))
	{
		engine.memory.widgets[widget]->parentUI = id;
		widgets.push_back(widget);
	}
}

bool KTech::UI::RemoveWidget(ID<Widget> widget)
{
	for (size_t i = 0; i < widgets.size(); i++)
	{
		if (widgets[i] == widget)
		{
			if (engine.memory.widgets.Exists(widgets[i]))
				engine.memory.widgets[widgets[i]]->parentUI = ID<UI>(0, 0);
			widgets.erase(widgets.begin() + i);
			return true;
		}
	}
	return false;
}

// Similar to `Camera::Render()`
void KTech::UI::Render()
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

	for (size_t w = 0; w < widgets.size(); w++)
	{
		KTech::Widget* widget = engine.memory.widgets[widgets[w]];
		
		for (size_t t = 0; t < widget->textures.size(); t++)
		{
			KTech::Texture& texture = widget->textures[t];
			
			if (!texture.active)
				continue;

			// Simple texture
			if (texture.simple)
			{
				// Precalculate fixed values (can only store 8-bit depth)
				tempFRGBA = RGBA(
				//  8 ->                16 ->               24 ->          8.
					texture.value.f.r * texture.value.f.a * alpha / 65025,
					texture.value.f.g * texture.value.f.a * alpha / 65025,
					texture.value.f.b * texture.value.f.a * alpha / 65025,
				//  8 ->                16 ->          8.
					texture.value.f.a * alpha / 255
				);
				tempBRGBA = RGBA(
					texture.value.b.r * texture.value.b.a * alpha / 65025,
					texture.value.b.g * texture.value.b.a * alpha / 65025,
					texture.value.b.b * texture.value.b.a * alpha / 65025,
					texture.value.b.a * alpha / 255
				);

				long y, x;
				// If the texture is before the camera, iterate with Y from point in which camera view starts, and iterate with final.y from 0.
				if (widget->pos.y + texture.pos_r.y < 0)
				{
					y = - widget->pos.y - texture.pos_r.y;
					final.y = 0;
				}
				// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
				else
				{
					y = 0;
					final.y = widget->pos.y + texture.pos_r.y;
				}
				
				// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
				for (; y < texture.size.y && final.y < res.y; y++, final.y++)
				{
					// Same goes for X
					if (widget->pos.x + texture.pos_r.x < 0)
					{
						x = - widget->pos.x - texture.pos_r.x;
						final.x = 0;
					}
					else
					{
						x = 0;
						final.x = widget->pos.x + texture.pos_r.x;
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
				if (widget->pos.y + texture.pos_r.y < 0)
				{
					y = - widget->pos.y - texture.pos_r.y;
					final.y = 0;
				}
				// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
				else
				{
					y = 0;
					final.y = widget->pos.y + texture.pos_r.y;
				}

				// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
				for (; y < texture.t.size() && final.y < res.y; y++, final.y++)
				{
					// Same goes for X
					if (widget->pos.x + texture.pos_r.x < 0)
					{
						x = - widget->pos.x - texture.pos_r.x;
						final.x = 0;
					}
					else
					{
						x = 0;
						final.x = widget->pos.x + texture.pos_r.x;
					}

					for (; x < texture.t[y].size() && final.x < res.x; x++, final.x++)
					{
						if (texture.t[y][x].c != ' ')
						{
							image[final.y][final.x].c = texture.t[y][x].c; // Character
							// Precalculate foreground * layer alpha (8 bit depth)
							//          8 ->                   16 ->          8.
							tempAlpha = texture.t[y][x].f.a * alpha / 255;
							//                            (8 ->                   16.         8 ->                           16.) 16 ->          8.
							image[final.y][final.x].f.r = (texture.t[y][x].f.r * tempAlpha + image[final.y][final.x].f.r * (255 - tempAlpha)) / 255;
							image[final.y][final.x].f.g = (texture.t[y][x].f.g * tempAlpha + image[final.y][final.x].f.g * (255 - tempAlpha)) / 255;
							image[final.y][final.x].f.b = (texture.t[y][x].f.b * tempAlpha + image[final.y][final.x].f.b * (255 - tempAlpha)) / 255;
							image[final.y][final.x].f.a += tempAlpha * (255 - image[final.y][final.x].f.a) / 255;
						}
						// Precalculate background * layer alpha (8 bit depth)
						//          8 ->                    16 ->                 8.
						tempAlpha = texture.t[y][x].b.a * alpha / 255;
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
	if (frgba.a != 0)
	{
		tempFRGBA = RGBA(frgba.a * frgba.r / 255, frgba.a * frgba.g / 255, frgba.a * frgba.b / 255, frgba.a);
		for (size_t y = 0; y < res.y; y++)
		{
			for (size_t x = 0; x < res.x; x++)
			{
				image[y][x].f.r = tempFRGBA.r + (255 - frgba.a) * image[y][x].f.r / 255;
				image[y][x].f.g = tempFRGBA.g + (255 - frgba.a) * image[y][x].f.g / 255;
				image[y][x].f.b = tempFRGBA.b + (255 - frgba.a) * image[y][x].f.b / 255;
				image[y][x].f.a += tempFRGBA.a * (255 - image[y][x].f.a) / 255;
			}
		}
	}
	if (brgba.a != 0)
	{
		tempBRGBA = RGBA(brgba.a * brgba.r / 255, brgba.a * brgba.g / 255, brgba.a * brgba.b / 255, brgba.a);
		for (size_t y = 0; y < res.y; y++)
		{
			for (size_t x = 0; x < res.x; x++)
			{
				image[y][x].b.r = tempBRGBA.r + (255 - brgba.a) * image[y][x].b.r / 255;
				image[y][x].b.g = tempBRGBA.g + (255 - brgba.a) * image[y][x].b.g / 255;
				image[y][x].b.b = tempBRGBA.b + (255 - brgba.a) * image[y][x].b.b / 255;
				image[y][x].b.a += tempBRGBA.a * (255 - image[y][x].b.a) / 255;
			}
		}
	}
}

void KTech::UI::Resize(UPoint newRes)
{
	static size_t y;
	res = newRes;
	image.resize(newRes.y);
	for (y = 0; y < newRes.y; y++)
		image[y].resize(newRes.x);
}