#pragma once

#include "../engine/engine.hpp"

struct Widget
{
	Engine::Object obj;
	bool selected = false;
	Engine::Input::CallbacksGroup callbackGroup{false};
	inline virtual void Select() {}
	inline virtual void Deselect() {}
	~Widget() { obj.parentLayer->RemoveObject(&obj); callbackGroup.DeleteCallbacks(); }
};