#include "engine.hpp"

Engine::Camera::Camera(Point position, UPoint resolution, const std::string& name)
	: pos(position), res(resolution)
{
	image.resize(res.y, std::vector<Cell>(res.x));
	// fAlphaMap.resize(res.y, std::vector<uint8_t>(res.x));
	// bAlphaMap.resize(res.y, std::vector<uint8_t>(res.x));
}

void Engine::Camera::Render(const std::vector<Layer*>& layers)
{
	static Engine::Layer* layer;
	static Engine::Object* obj;
	static Engine::Texture* texture;
	static long t, o, l, x, y;

	// Point final - the relative position between the camera and the texture (image index).
	static Engine::Point final;
	static RGB tempFRGB;
	static RGB tempBRGB;
	static uint8_t tempAlpha;

	// Reset the image.
	for (y = 0; y < res.y; y++)
	{
		for (x = 0; x < res.x; x++)
		{
			image[y][x].f = { 0, 0, 0 };
			image[y][x].b = { 0, 0, 0 };
			image[y][x].c = ' ';
		}
	}

	for (l = 0; l < layers.size(); l++)
	{
		if (!layers[l]->visible)
			continue;

		layer = layers[l];

		for (o = 0; o < layer->objects.size(); o++)
		{
			obj = layer->objects[o];
			for (t = 0; t < obj->textures.size(); t++)
			{
				texture = &obj->textures[t];
				
				if (!texture->active)
					continue;

				// Simple texture
				if (texture->simple)
				{
					// Precalculate fixed values (8 bit depth)
					//             8 ->                 16 ->                32 ->          8.     8 ->                 16 ->                32 ->          8.     8 ->                 16 ->                32 ->          8.
					tempFRGB = RGB(texture->value.f.r * texture->value.f.a * layer->alpha / 65025, texture->value.f.g * texture->value.f.a * layer->alpha / 65025, texture->value.f.b * texture->value.f.a * layer->alpha / 65025);
					tempBRGB = RGB(texture->value.b.r * texture->value.b.a * layer->alpha / 65025, texture->value.b.g * texture->value.b.a * layer->alpha / 65025, texture->value.b.b * texture->value.b.a * layer->alpha / 65025);
					//                   8 ->                 16 ->           8.
					tempAlpha = (65025 - texture->value.f.a * layer->alpha) / 255;

					// If the texture is before the camera, iterate with Y from point in which camera view starts, and iterate with final.y from 0.
					if (obj->pos.y + texture->pos_r.y < pos.y)
					{
						y = pos.y - obj->pos.y - texture->pos_r.y;
						final.y = 0;
					}
					// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
					else
					{
						y = 0;
						final.y = obj->pos.y + texture->pos_r.y - pos.y;
					}
					// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
					for (; y < texture->size.y && final.y < res.y; y++, final.y++)
					{
						// Same goes for X
						if (obj->pos.x + texture->pos_r.x < pos.x)
						{
							x = pos.x - obj->pos.x - texture->pos_r.x;
							final.x = 0;
						}
						else
						{
							x = 0;
							final.x = obj->pos.x + texture->pos_r.x - pos.x;
						}

						for (; x < texture->size.x && final.x < res.x; x++, final.x++)
						{
							if (texture->value.c != ' ')
							{
								image[final.y][final.x].c = texture->value.c;
								//                            8.           8 ->                          16 ->       8.
								image[final.y][final.x].f.r = tempFRGB.r + image[final.y][final.x].f.r * tempAlpha / 255;
								image[final.y][final.x].f.g = tempFRGB.g + image[final.y][final.x].f.g * tempAlpha / 255;
								image[final.y][final.x].f.b = tempFRGB.b + image[final.y][final.x].f.b * tempAlpha / 255;
							}
							//                            8.           8 ->                          16 ->       8.
							image[final.y][final.x].b.r = tempBRGB.r + image[final.y][final.x].b.r * tempAlpha / 255;
							image[final.y][final.x].b.g = tempBRGB.g + image[final.y][final.x].b.g * tempAlpha / 255;
							image[final.y][final.x].b.b = tempBRGB.b + image[final.y][final.x].b.b * tempAlpha / 255;
						}
					}
				}
				// Complex texture
				else
				{
					// If the texture is before the camera, iterate with Y from point in which camera view starts, and iterate with final.y from 0.
					if (obj->pos.y + texture->pos_r.y < pos.y)
					{
						y = pos.y - obj->pos.y - texture->pos_r.y;
						final.y = 0;
					}
					// Otherwise, iterate with Y from 0, and iterate with final.y from point in which texture starts.
					else
					{
						y = 0;
						final.y = obj->pos.y + texture->pos_r.y - pos.y;
					}

					// Stop iterating through the texture until y reached the end of the texture, or final.y reached the end of the image.
					for (; y < texture->t.size() && final.y < res.y; y++, final.y++)
					{
						// Same goes for X
						if (obj->pos.x + texture->pos_r.x < pos.x)
						{
							x = pos.x - obj->pos.x - texture->pos_r.x;
							final.x = 0;
						}
						else
						{
							x = 0;
							final.x = obj->pos.x + texture->pos_r.x - pos.x;
						}

						for (; x < texture->t[y].size() && final.x < res.x; x++, final.x++)
						{
							if (texture->t[y][x].c != ' ')
							{
								image[final.y][final.x].c = texture->t[y][x].c; // Character
								// Precalculate foreground * layer alpha (8 bit depth)
								//          8 ->                   16 ->          8.
								tempAlpha = texture->t[y][x].f.a * layer->alpha / 255;
								//                            (8 ->                   16.         8 ->                           16.) 16 ->          8.
								image[final.y][final.x].f.r = (texture->t[y][x].f.r * tempAlpha + image[final.y][final.x].f.r * (255 - tempAlpha)) / 255;
								image[final.y][final.x].f.g = (texture->t[y][x].f.g * tempAlpha + image[final.y][final.x].f.g * (255 - tempAlpha)) / 255;
								image[final.y][final.x].f.b = (texture->t[y][x].f.b * tempAlpha + image[final.y][final.x].f.b * (255 - tempAlpha)) / 255;
							}
							// Precalculate background * layer alpha (8 bit depth)
							//          8 ->                    16 ->                 8.
							tempAlpha = texture->t[y][x].b.a * layer->alpha / 255;
							//                            (8 ->                   16.         8 ->                           16.) 16 ->          8.
							image[final.y][final.x].b.r = (texture->t[y][x].b.r * tempAlpha + image[final.y][final.x].b.r * (255 - tempAlpha)) / 255;
							image[final.y][final.x].b.g = (texture->t[y][x].b.g * tempAlpha + image[final.y][final.x].b.g * (255 - tempAlpha)) / 255;
							image[final.y][final.x].b.b = (texture->t[y][x].b.b * tempAlpha + image[final.y][final.x].b.b * (255 - tempAlpha)) / 255;
						}
					}
				}
			}
		}

		if (layer->brgba.a != 0)
		{
			for (size_t y = 0; y < res.y; y++)
			{
				for (size_t x = 0; x < res.x; x++)
				{
					image[y][x].b.r = layer->brgba.a * layer->brgba.r + (1.0f - layer->brgba.a) * image[y][x].b.r;
					image[y][x].b.g = layer->brgba.a * layer->brgba.g + (1.0f - layer->brgba.a) * image[y][x].b.g;
					image[y][x].b.b = layer->brgba.a * layer->brgba.b + (1.0f - layer->brgba.a) * image[y][x].b.b;
				}
			}
		}
		
		if (layer->frgba.a != 0)
		{
			for (size_t y = 0; y < res.y; y++)
			{
				for (size_t x = 0; x < res.x; x++)
				{
					image[y][x].f.r = layer->frgba.a * layer->frgba.r + (1.0f - layer->frgba.a) * image[y][x].f.r;
					image[y][x].f.g = layer->frgba.a * layer->frgba.g + (1.0f - layer->frgba.a) * image[y][x].f.g;
					image[y][x].f.b = layer->frgba.a * layer->frgba.b + (1.0f - layer->frgba.a) * image[y][x].f.b;
				}
			}
		}
	}
}

/*
void Engine::Camera::RenderReversed(std::vector<Layer*> layers)
{
	// Reset the image and the alpha maps.
	for (y = 0; y < res.y; y++)
	{
		for (x = 0; x < res.x; x++)
		{
			image[y][x].f = { 0, 0, 0 };
			image[y][x].b = { 0, 0, 0 };
			image[y][x].c = ' ';
			fAlphaMap[y][x] = 255;
			bAlphaMap[y][x] = 255;
		}
	}
	static uint32_t fAlphaMapCounter = 0;
	static uint32_t bAlphaMapCounter = 0;
	fAlphaMapCounter = res.x * res.y;
	bAlphaMapCounter = fAlphaMapCounter;

	// std::cerr << "--- New Print:" << '\n';
	// std::cerr << "F alphaMapCounter:" << fAlphaMapCounter << '\n';
	// std::cerr << "B alphaMapCounter:" << bAlphaMapCounter << '\n';

	// --== FOREGROUND ==--
	for (l = layers.size() - 1; l >= 0 && fAlphaMapCounter > 0; l--)
	{
		if (!layers[l]->visible)
			continue;

		layer = layers[l];

		// `&& alphaMapCounter != 0` - proceed with rendering objects only if there are more free cells on the image.
		for (o = layer->objects.size() - 1; o >= 0 && fAlphaMapCounter > 0; o--)
		{
			obj = layer->objects[o];
			// `&& alphaMapCounter != 0` - proceed with rendering textures only if there are more free cells on the image.
			for (t = obj->textures.size() - 1; t >= 0 && fAlphaMapCounter > 0; t--)
			{
				texture = &obj->textures[t];
				
				if (!texture->active)
					continue;

				if (texture->simple)
				{
					final.y = obj->pos.y + texture->pos_r.y - pos.y;
					for (y = (final.y < 0 ? 0 - final.y : 0); y < texture->size.y && final.y < res.y; y++, final.y++)
					{
						final.x = obj->pos.x + texture->pos_r.x - pos.x;
						for (x = (final.x < 0 ? 0 - final.x : 0); x < texture->size.x && final.x < res.x; x++, final.x++)
						{
							if (image[final.y][final.x].c == ' ')
								image[final.y][final.x].c = texture->value.c;
							// Check if the cell is already filled
							if (fAlphaMap[final.y][final.x] > 0)
							{
								image[final.y][final.x].f.r += texture->value.f.r * texture->value.f.a * fAlphaMap[final.y][final.x] / 65025;
								image[final.y][final.x].f.g += texture->value.f.g * texture->value.f.a * fAlphaMap[final.y][final.x] / 65025;
								image[final.y][final.x].f.b += texture->value.f.b * texture->value.f.a * fAlphaMap[final.y][final.x] / 65025;
								fAlphaMap[final.y][final.x] = (255 - texture->value.f.a) * fAlphaMap[final.y][final.x] / 255;
								// Update alphaMap is the cell is filled (could be optimized by not comparing it each cell)
								if (fAlphaMap[final.y][final.x] == 0)
									fAlphaMapCounter--;
							}
						}
					}
				}
				else
				{
					final.y = obj->pos.y + texture->pos_r.y - pos.y;
					for (y = (final.y < 0 ? 0 - final.y : 0); y < texture->t.size() && final.y < res.y; y++, final.y++)
					{
						final.x = obj->pos.x + texture->pos_r.x - pos.x;
						for (x = (final.x < 0 ? 0 - final.x : 0); x < texture->t[y].size() && final.x < res.x; x++, final.x++)
						{
							if (image[final.y][final.x].c == ' ')
								image[final.y][final.x].c = texture->t[y][x].c;
							// Check if the cell is already filled
							if (fAlphaMap[final.y][final.x] > 0)
							{
								image[final.y][final.x].f.r += texture->t[y][x].f.r * texture->t[y][x].f.a * fAlphaMap[final.y][final.x] / 65025;
								image[final.y][final.x].f.g += texture->t[y][x].f.g * texture->t[y][x].f.a * fAlphaMap[final.y][final.x] / 65025;
								image[final.y][final.x].f.b += texture->t[y][x].f.b * texture->t[y][x].f.a * fAlphaMap[final.y][final.x] / 65025;
								fAlphaMap[final.y][final.x] = (255 - texture->t[y][x].f.a) * fAlphaMap[final.y][final.x] / 255;
								// Update alphaMap is the cell is filled (could be optimized by not comparing it each cell)
								if (fAlphaMap[final.y][final.x] == 0)
									fAlphaMapCounter--;
							}
						}
					}
				}
			}
		}
		
		if (layer->frgba.a != 0)
		{
			for (size_t y = 0; y < res.y; y++)
			{
				for (size_t x = 0; x < res.x; x++)
				{
					image[y][x].f.r = layer->frgba.a * layer->frgba.r + (1.0f - layer->frgba.a) * image[y][x].f.r;
					image[y][x].f.g = layer->frgba.a * layer->frgba.g + (1.0f - layer->frgba.a) * image[y][x].f.g;
					image[y][x].f.b = layer->frgba.a * layer->frgba.b + (1.0f - layer->frgba.a) * image[y][x].f.b;
				}
			}
		}
	}

	// --== BACKGROUND ==--
	for (l = layers.size() - 1; l >= 0 && bAlphaMapCounter > 0; l--)
	{
		if (!layers[l]->visible)
			continue;

		layer = layers[l];

		// `&& alphaMapCounter != 0` - proceed with rendering objects only if there are more free cells on the image.
		for (o = layer->objects.size() - 1; o >= 0 && bAlphaMapCounter > 0; o--)
		{
			obj = layer->objects[o];
			// `&& alphaMapCounter != 0` - proceed with rendering textures only if there are more free cells on the image.
			for (t = obj->textures.size() - 1; t >= 0 && bAlphaMapCounter > 0; t--)
			{
				texture = &obj->textures[t];
				
				if (!texture->active)
					continue;

				if (texture->simple)
				{
					final.y = obj->pos.y + texture->pos_r.y - pos.y;
					for (y = (final.y < 0 ? 0 - final.y : 0); y < texture->size.y && final.y < res.y; y++, final.y++)
					{
						final.x = obj->pos.x + texture->pos_r.x - pos.x;
						for (x = (final.x < 0 ? 0 - final.x : 0); x < texture->size.x && final.x < res.x; x++, final.x++)
						{
							if (image[final.y][final.x].c == ' ')
								image[final.y][final.x].c = texture->value.c;
							// Check if the cell is already filled
							if (bAlphaMap[final.y][final.x] > 0)
							{
								// It isn't, then draw
								image[final.y][final.x].b.r += texture->value.b.r * texture->value.b.a * bAlphaMap[final.y][final.x] / 65025;
								image[final.y][final.x].b.g += texture->value.b.g * texture->value.b.a * bAlphaMap[final.y][final.x] / 65025;
								image[final.y][final.x].b.b += texture->value.b.b * texture->value.b.a * bAlphaMap[final.y][final.x] / 65025;
								bAlphaMap[final.y][final.x] = (255 - texture->value.b.a) * bAlphaMap[final.y][final.x] / 255;
								// Update alphaMap is the cell is filled (could be optimized by not comparing it each cell)
								if (bAlphaMap[final.y][final.x] == 0)
									bAlphaMapCounter--;
							}
						}
					}
				}
				else
				{
					final.y = obj->pos.y + texture->pos_r.y - pos.y;
					for (y = (final.y < 0 ? 0 - final.y : 0); y < texture->t.size() && final.y < res.y; y++, final.y++)
					{
						final.x = obj->pos.x + texture->pos_r.x - pos.x;
						for (x = (final.x < 0 ? 0 - final.x : 0); x < texture->t[y].size() && final.x < res.x; x++, final.x++)
						{
							if (image[final.y][final.x].c == ' ')
								image[final.y][final.x].c = texture->t[y][x].c;
							// Check if the cell is already filled
							if (bAlphaMap[final.y][final.x] > 0)
							{
								image[final.y][final.x].b.r += texture->t[y][x].b.r * texture->t[y][x].b.a * bAlphaMap[final.y][final.x] / 65025;
								image[final.y][final.x].b.g += texture->t[y][x].b.g * texture->t[y][x].b.a * bAlphaMap[final.y][final.x] / 65025;
								image[final.y][final.x].b.b += texture->t[y][x].b.b * texture->t[y][x].b.a * bAlphaMap[final.y][final.x] / 65025;
								bAlphaMap[final.y][final.x] = (255 - texture->t[y][x].b.a) * bAlphaMap[final.y][final.x] / 255;
								// Update alphaMap is the cell is filled (could be optimized by not comparing it each cell)
								if (bAlphaMap[final.y][final.x] == 0)
									bAlphaMapCounter--;
							}
						}
					}
				}
			}
		}

		if (layer->brgba.a != 0)
		{
			for (size_t y = 0; y < res.y; y++)
			{
				for (size_t x = 0; x < res.x; x++)
				{
					image[y][x].b.r = layer->brgba.a * layer->brgba.r + (1.0f - layer->brgba.a) * image[y][x].b.r;
					image[y][x].b.g = layer->brgba.a * layer->brgba.g + (1.0f - layer->brgba.a) * image[y][x].b.g;
					image[y][x].b.b = layer->brgba.a * layer->brgba.b + (1.0f - layer->brgba.a) * image[y][x].b.b;
				}
			}
		}
	}

	// std::cerr << "F alphaMapCounter:" << fAlphaMapCounter << '\n';
	// std::cerr << "B alphaMapCounter:" << bAlphaMapCounter << '\n';
}*/

void Engine::Camera::Draw(Point pos, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) const
{
	// Return if the image is sized 0
	if (image.size() == 0)
		return;
	if (image[0].size() == 0)
		return;

	// Default the rectangle
	if (bottom == 0)
		bottom = image.size();
	if (right == 0)
		right = image[0].size();
	// Return if rectangle is invalid
	if (left >= right || top >= bottom)
		return;

	// The assumption is that Engine::image and Camera::image are not evenly sized

	long ySkip = 0;
	long xSkip = 0;
	if (pos.y < 0)
		ySkip = -pos.y;
	if (pos.x < 0)
		xSkip = -pos.x;
	
	// Draw
	// y - camera's Y, yE - engine's Y
	for (long y = top + ySkip, yE = pos.y + ySkip;
		y < bottom && y < image.size() && yE < Engine::image.size(); 
		y++, yE++)
	{
		for (long x = left + xSkip, xE = pos.x + xSkip;
			x < right && x < image[y].size() && xE < Engine::image[yE].size();
			x++, xE++)
		{
			Engine::image[y - top + pos.y][x - left + pos.x] = image[y][x];
		}
	}
}

void Engine::Camera::Resize(UPoint _res)
{
	static size_t y;
	res = _res;
	image.resize(_res.y);
	// fAlphaMap.resize(_res.y);
	// bAlphaMap.resize(_res.y);
	for (y = 0; y < _res.y; y++)
	{
		image[y].resize(_res.x);
		// fAlphaMap[y].resize(_res.x);
		// bAlphaMap[y].resize(_res.x);
	}	
}
