/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 Ethan Kaufman (AKA Kaup)

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

#pragma once

#include "../ktech.hpp"

struct Widget
{
	KTech::Object obj;
	bool selected = false;
	KTech::IO::CallbacksGroup& callbacksGroup;
	inline virtual void Select() {}
	inline virtual void Deselect() {}
	inline Widget(KTech::Layer* layer, KTech::Point pos) : obj(pos, layer), callbacksGroup(obj.parentLayer->parentMap->parentEngine->io.CreateCallbackGroup(false)) { }
	inline ~Widget() { callbacksGroup.DeleteCallbacks(); }
};
