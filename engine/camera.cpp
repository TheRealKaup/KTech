#include "engine.hpp"

Engine::Camera::Camera(Point position, UPoint resolution, std::string name)
	: pos(position), res(resolution) {}

// <400 before, >400 after second update, >1500 expected max potential before, 1300-2000 now. (20x20)

static Engine::Layer* layer;
static Engine::Object* obj;
static Engine::Texture* texture;
static int t, o, l, x, y, finalY, finalX, baseX;
static u_int32_t temp;

void Engine::Camera::Render(std::vector<Layer*> layers)
{
	if (image.size() != res.y)
		image.resize(res.y, std::vector<Cell>(res.x));
	for (y = 0; y < res.y; y++)
	{
		for (x = 0; x < res.x; x++)
		{
			image[y][x].frgb = { 0, 0, 0 };
			image[y][x].brgb = { 0, 0, 0 };
			image[y][x].character = ' ';
		}
	}

	for (size_t i = 0; i < layers.size(); i++)
	{
		if (!layers[i]->active)
			continue;

		layer = layers[i];

		for (o = 0; o < layer->objects.size(); o++)
		{
			obj = layer->objects[o];
			for (t = 0; t < obj->textures.size(); t++)
			{
				texture = &obj->textures[t];
				
				if (!texture->active)
					continue;

				for (y = 0; y < texture->t.size(); y++)
				{
					finalY = y + obj->pos.y + texture->pos.y - pos.y;

					if (finalY < 0)
						continue;
					if (finalY >= res.y)
						break;

					for (x = 0; x < texture->t[y].size(); x++)
					{
						finalX = x + obj->pos.x + texture->pos.x - pos.x;

						if (finalX < 0)
							continue;
						if (finalX >= res.x)
							break;

						if (texture->t[y][x].character != ' ')
						{
							image[finalY][finalX].character = texture->t[y][x].character;
							image[finalY][finalX].frgb.r = (texture->t[y][x].frgba.r * texture->t[y][x].frgba.a + image[finalY][finalX].frgb.r * (255 - texture->t[y][x].frgba.a)) / 255;
							image[finalY][finalX].frgb.g = (texture->t[y][x].frgba.g * texture->t[y][x].frgba.a + image[finalY][finalX].frgb.g * (255 - texture->t[y][x].frgba.a)) / 255;
							image[finalY][finalX].frgb.b = (texture->t[y][x].frgba.b * texture->t[y][x].frgba.a + image[finalY][finalX].frgb.b * (255 - texture->t[y][x].frgba.a)) / 255;;
						}
						image[finalY][finalX].brgb.r = (texture->t[y][x].brgba.r * texture->t[y][x].brgba.a + image[finalY][finalX].brgb.r * (255 - texture->t[y][x].brgba.a)) / 255;
						image[finalY][finalX].brgb.g = (texture->t[y][x].brgba.g * texture->t[y][x].brgba.a + image[finalY][finalX].brgb.g * (255 - texture->t[y][x].brgba.a)) / 255;
						image[finalY][finalX].brgb.b = (texture->t[y][x].brgba.b * texture->t[y][x].brgba.a + image[finalY][finalX].brgb.b * (255 - texture->t[y][x].brgba.a)) / 255;
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
					image[y][x].brgb.r = layer->brgba.a * layer->brgba.r + (1.0f - layer->brgba.a) * image[y][x].brgb.r;
					image[y][x].brgb.g = layer->brgba.a * layer->brgba.g + (1.0f - layer->brgba.a) * image[y][x].brgb.g;
					image[y][x].brgb.b = layer->brgba.a * layer->brgba.b + (1.0f - layer->brgba.a) * image[y][x].brgb.b;
				}
			}
		}
		
		if (layer->frgba.a != 0)
		{
			for (size_t y = 0; y < res.y; y++)
			{
				for (size_t x = 0; x < res.x; x++)
				{
					image[y][x].frgb.r = layer->frgba.a * layer->frgba.r + (1.0f - layer->frgba.a) * image[y][x].frgb.r;
					image[y][x].frgb.g = layer->frgba.a * layer->frgba.g + (1.0f - layer->frgba.a) * image[y][x].frgb.g;
					image[y][x].frgb.b = layer->frgba.a * layer->frgba.b + (1.0f - layer->frgba.a) * image[y][x].frgb.b;
				}
			}
		}
	}
}

void Engine::Camera::Draw(Point pos, unsigned left, unsigned top, unsigned right, unsigned bottom)
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
	if (left > right || top > bottom)
		return; // Maybe reversed?

	// The assumption is that Engine::image and Camera::image are not evenly sized

	int ySkip = 0;
	int xSkip = 0;
	if (pos.y < 0)
		ySkip = -pos.y;
	if (pos.x < 0)
		xSkip = -pos.x;
	
	// Draw
	// y - camera's Y, yE - engine's Y
	for (int y = top + ySkip, yE = pos.y + ySkip;
		y < bottom && y < image.size() && yE < Engine::image.size(); 
		y++, yE++)
	{
		for (int x = left + xSkip, xE = pos.x + xSkip;
			x < right && x < image[y].size() && xE < Engine::image[yE].size();
			x++, xE++)
		{
			Engine::image[y - top + pos.y][x - left + pos.x] = image[y][x];
		}
	}
}
