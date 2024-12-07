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
#include "../basic/cell.hpp"
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"

#include <limits>
#include <string>
#include <vector>

/*!
	World structure that renders `Object`s of `Layer`s, and exists within `Map`.

	`Camera` is able to render a `Cell`-based image (`Camera::Render()`), which can be drawn to `Output`'s image buffer (`Camera::Draw()` or `Output::Draw()`), and printed to the terminal (`Output::Print()`).
*/
class KTech::Camera
{
public:
	Engine& engine; //!< Parent engine.
	const ID<Camera> m_id{ID<Camera>::Unique()}; //!< Personal `ID`.
	std::string m_name; //!< String name, might be useful for debugging.
	ID<Map> m_parentMap; //!< The map which contains this `Camera`.

	Point m_pos; //!< World position.
	UPoint m_res; //!< `Camera::m_image`'s resolution (or "size").
	Cell m_background = Cell(' ', RGB(0, 0, 0), RGB(0, 0, 0)); //!< The background to render upon.
	std::vector<Cell> m_image; //!< `Cell`-based rendered image.

	Camera(Engine& engine, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
	Camera(Engine& engine, const ID<Map>& parentMap, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = "");
	virtual ~Camera();

	auto EnterMap(const ID<Map>& map) -> bool;
	auto LeaveMap() -> bool;

	void Resize(UPoint resolution);

	void Render();
	void Render(const std::vector<ID<Layer>>& layers);
	void Draw(Point position = Point(0, 0), UPoint start = UPoint(0, 0), UPoint end = UPoint(0, 0), uint8_t alpha = std::numeric_limits<uint8_t>::max());

protected:
	virtual auto OnTick() -> bool;

private:
	inline void RenderBackground();
	inline void RenderSimple(uint8_t layerAlpha, Object* object, Texture& texture);
	inline void RenderComplex(uint8_t layerAlpha, Object* object, Texture& texture);
	inline void RenderForeground(const RGBA& frgba, const RGBA& brgba);

	friend class KTech::Memory;
};