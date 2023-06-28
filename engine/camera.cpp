#include "engine.hpp"

Engine::Camera::Camera(Point position, UPoint resolution, const std::string& name)
	: pos(position), res(resolution) {}

// <400 before, >400 after second update, >1500 expected max potential before, 1300-2000 now. (20x20)

static Engine::Layer* layer;
static Engine::Object* obj;
static Engine::Texture* texture;
static size_t t, o, l, x, y;
static Engine::Point final;
static u_int32_t temp;

void Engine::Camera::Render(std::vector<Layer*> layers)
{
	if (image.size() != res.y)
		image.resize(res.y, std::vector<Cell>(res.x));
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
		if (!layers[l]->active)
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

				for (y = 0; y < texture->t.size(); y++)
				{
					final.y = y + obj->pos.y + texture->pos.y - pos.y;

					if (final.y < 0)
						continue;
					if (final.y >= res.y)
						break;

					for (x = 0; x < texture->t[y].size(); x++)
					{
						final.x = x + obj->pos.x + texture->pos.x - pos.x;

						if (final.x < 0)
							continue;
						if (final.x >= res.x)
							break;

						if (texture->t[y][x].c != ' ')
						{
							// image[finalY][finalX].frgb.r = layer->opacity * text->t[y][x].frgba.a * text->t[y][x].frgba.r + (1.0f - layer->opacity * text->t[y][x].frgba.a) * image[finalY][finalX].frgb.r;
							image[final.y][final.x].c = texture->t[y][x].c;
							image[final.y][final.x].f.r = (texture->t[y][x].f.r * texture->t[y][x].f.a * layer->alpha + image[final.y][final.x].f.r * (65025 - texture->t[y][x].f.a * layer->alpha)) / 65025;
							image[final.y][final.x].f.g = (texture->t[y][x].f.g * texture->t[y][x].f.a * layer->alpha + image[final.y][final.x].f.g * (65025 - texture->t[y][x].f.a * layer->alpha)) / 65025;
							image[final.y][final.x].f.b = (texture->t[y][x].f.b * texture->t[y][x].f.a * layer->alpha + image[final.y][final.x].f.b * (65025 - texture->t[y][x].f.a * layer->alpha)) / 65025;;
						}
						image[final.y][final.x].b.r = (texture->t[y][x].b.r * texture->t[y][x].b.a * layer->alpha + image[final.y][final.x].b.r * (65025 - texture->t[y][x].b.a * layer->alpha)) / 65025;
						image[final.y][final.x].b.g = (texture->t[y][x].b.g * texture->t[y][x].b.a * layer->alpha + image[final.y][final.x].b.g * (65025 - texture->t[y][x].b.a * layer->alpha)) / 65025;
						image[final.y][final.x].b.b = (texture->t[y][x].b.b * texture->t[y][x].b.a * layer->alpha + image[final.y][final.x].b.b * (65025 - texture->t[y][x].b.a * layer->alpha)) / 65025;
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

void Engine::Camera::Draw(Point pos, uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
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
