#pragma once

#include "../engine/engine.hpp"

struct Widget
{
	Engine::Object obj;
	bool selected = false;
	inline virtual void Select() {}
	inline virtual void Deselect() {}
};