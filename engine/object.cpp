#include "engine.hpp"

void Engine::Object::EnterLayer(Engine::Layer* layer)
{
	if (parentLayer != nullptr)
		parentLayer->RemoveObject(this);
	layer->AddObject(this);
}

Engine::Object::Object(Point pos, const std::string& name) : pos(pos), name(name) {}

Engine::Object::~Object()
{
	if (parentLayer)
		parentLayer->RemoveObject(this);
}