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
#include "../utility/id.hpp"
#include "../basic/cella.hpp"
#include "../basic/rgba.hpp"
#include "../basic/upoint.hpp"

#include <string>
#include <vector>

// Acts as a camera and a layer for `Widget`s. Image is compatible with `IO::Draw()`.
class KTech::UI
{
public:
	Engine& engine;
	ID<UI> m_id;
	std::string m_name;
	std::vector<ID<Widget>> m_widgets;

	UPoint m_res;
	CellA m_background = CellA(' ', RGBA(0, 0, 0, 0), RGBA(0, 0, 0, 0)); // The background to render upon.
	RGBA m_frgba = RGBA(0, 0, 0, 0);
	RGBA m_brgba = RGBA(0, 0, 0, 0);
	uint8_t m_alpha = 255;
	std::vector<CellA> m_image;

	UI(Engine& engine, UPoint resolution = UPoint(10, 10), const std::string& name = "");
	virtual ~UI();

	bool AddWidget(ID<Widget> widget);
	bool RemoveWidget(ID<Widget> widget);
	bool RemoveAllWidgets();

	void Resize(UPoint resolution);

	void Render();
	
protected:
	inline virtual bool OnTick() { return false; };

	friend class KTech::Memory;
};