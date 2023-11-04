#include "engine.hpp"

void Engine::Object::EnterLayer(Engine::Layer* layer)
{
	if (parentLayer != nullptr)
		parentLayer->RemoveObject(this);
	layer->AddObject(this);
}

Engine::Object::Object(Point pos, Layer* layer, const std::string& name)
	: pos(pos), name(name)
{
	if(layer)
		EnterLayer(layer);
}

Engine::Object::~Object()
{
	if (parentLayer)
		parentLayer->RemoveObject(this);
}