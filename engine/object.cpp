#include "engine.hpp"

Engine::Object::Object(Point pos, const std::string& name) : pos(pos), name(name) {}

Engine::Object::~Object()
{
	if (parentLayer)
		parentLayer->RemoveObject(this);
}