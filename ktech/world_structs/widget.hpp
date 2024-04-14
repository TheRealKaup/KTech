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

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION
#include "../misc/id.hpp"
#include "../basic_structs/point.hpp"
#include "../world_structs/texture.hpp"
#include "../engine/io.hpp"

// Widget is now a non-optional KTech standard
struct KTech::Widget
{
	Engine& engine;
	ID<Widget> id;
	std::string name = "";
	KTech::ID<KTech::UI> parentUI;

	Point pos = Point(0, 0);
	
	std::vector<Texture> textures = {};
	IO::CallbacksGroup* callbacksGroup;
	bool selected = false;
	bool shown = true;

	Widget(Engine& engine, ID<UI> parentUI, Point pos);
	~Widget();

	inline virtual void RenderSelected () {}
	inline virtual void RenderUnselected () {}
	void Select();
	void Deselect();

	void EnterUI(ID<UI> ui);

	inline virtual void OnTick() {};
};