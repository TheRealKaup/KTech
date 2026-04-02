/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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
#include "../basic/cell.hpp"
#include "../basic/point.hpp"
#include "../basic/upoint.hpp"
#include "../utility/id.hpp"

#include <limits>
#include <string>
#include <vector>

/*!
	@brief World structure that renders `Object`s of `Layer`s, and exists within `Map`.

	`Camera` is able to render a `Cell`-based image (`Camera::Render()`), which can be drawn to `Output`'s image buffer (`Camera::Draw()` or `Output::Draw()`), and printed to the terminal (`Output::Print()`).
*/
class KTech::Camera
{
public:
	Engine& engine;								 //!< Parent engine.
	const ID<Camera> m_id{ID<Camera>::Unique()}; //!< Personal `ID`.
	std::string m_name;							 //!< String name, might be useful for debugging.
	ID<Map> m_parentMap;						 //!< The map which contains this `Camera`.

	Point m_pos;											   //!< World position.
	UPoint m_res;											   //!< `Camera::m_image`'s resolution (or "size").
	Cell m_background = Cell(' ', RGB(0, 0, 0), RGB(0, 0, 0)); //!< The background to render upon.
	std::vector<Cell> m_image;								   //!< `Cell`-based rendered image.

	/*!
		@fn Camera::Camera(Engine &engine, Point position=Point(0, 0), UPoint resolution=UPoint(10, 10), const std::string &name="")
		@brief Prepare `Camera` for rendering.

		@param [in] engine Parent engine.
		@param [in] position World position.
		@param [in] resolution Image resolution.
		@param [in] name String name.
	*/
	Camera(
		Engine& engine, Point position = Point(0, 0), UPoint resolution = UPoint(10, 10), const std::string& name = ""
	);

	/*!
		@fn Camera::Camera(Engine& engine, const ID<Map>& parentMap, Point position=Point(0, 0), UPoint resolution=UPoint(10, 10), const std::string &name="")
		@brief Prepare `Camera` for rendering and immediately enter a `Map`.

		@param [in] engine Parent engine.
		@param [in] parentMap Parent map to immediately enter.
		@param [in] position World position.
		@param [in] resolution Image resolution.
		@param [in] name String name.

		@see `Map::m_activeCameraI`
	*/
	Camera(
		Engine& engine,
		const ID<Map>& parentMap,
		Point position = Point(0, 0),
		UPoint resolution = UPoint(10, 10),
		const std::string& name = ""
	);

	/*!
		@brief Leave the parent map (if in one) and removed itself from `Memory`.
	*/
	virtual ~Camera();

	/*!
		@fn Camera::EnterMap
		@brief Enter a parent `Map`.

		@param map Parent map to enter.

		@return True if joined given `Map`. False if already in given `Map`, given `Map` doesn't exist in `Memory`, or failed to join.

		@see `Map::m_activeCameraI`
	*/
	auto EnterMap(const ID<Map>& map) -> bool;

	/*!
		@brief Leave the parent `Map`.

		@return True if left `Camera::m_parentMap`. False if doesn't have a parent `Map`, given `Map` doesn't exist in `Memory`, or failed to leave.

		@see `Map::m_activeCameraI`
	*/
	auto LeaveMap() -> bool;

	/*!
		@fn Camera::Resize
		@brief Resize the image resolution (or "size").

		@param resolution The new resolution.
	*/
	void Resize(UPoint resolution);

	/*!
		@brief Render all `Object`s of all `Layer`s of the parent `Map`.
	*/
	void Render();

	/*!
		@fn Camera::Render(const std::vector<ID<Layer>>& layers)
		@brief Render all `Object`s of the given `Layer`s.

		@param layers The `Layer`s containing the `Object`s to render.
	*/
	void Render(const std::vector<ID<Layer>>& layers);

	/*!
		@fn Camera::Draw
		@brief Draw the rendered image (`Camera::m_image`) to `Output` so it can be printed to the terminal.

		This function redirects to `Output::Draw()` and passes the given parameters verbatim.

		@see `Output::Draw()` for parameters explanation.
	*/
	void Draw(
		Point position = Point(0, 0),
		UPoint start = UPoint(0, 0),
		UPoint end = UPoint(0, 0),
		uint8_t alpha = std::numeric_limits<uint8_t>::max()
	);

	/*!
		@brief Shortcut for `Camera::Render()`, `Camera::Draw()` and `Output::Print()`.

		This function calls the above functions with respect to "render on demand" (by checking `Output::ShouldRenderThisTick()` and `Output::ShouldPrintThisTick()`). So, you can use this function in your game loop to avoid boilerplate code while still maintaining good performance, unless you want more functionality in your graphics portion of your game loop. This function is especially convenient for testing in no-game-loop mode.

		@see `Camera::Render()`
		@see `Camera::Draw()`
		@see `Output::Print()`
		@see `Engine::noGameLoopMode`
		@see [Tutorial chapter 5](https://github.com/TheRealKaup/KTech/blob/master/documentation/tutorial/tutorial.md#chapters): no-game-loop mode example that uses this function
	*/
	void RenderDrawPrint();

protected:
	/*!
		@brief Virtual function called once each tick.

		You can override this in your inherited class to add whatever functionality you want.

		Called by `Memory::CallOnTicks()`.

		@return `bool` value, which is explained in `Output::ShouldRenderThisTick()`.

		@see `Memory::CallOnTicks()`
		@see `Output::ShouldRenderThisTick()`
	*/
	virtual auto OnTick() -> bool;

private:
	inline void RenderBackground();
	inline void RenderSimple(uint8_t layerAlpha, Object* object, Texture& texture);
	inline void RenderComplex(uint8_t layerAlpha, Object* object, Texture& texture);
	inline void RenderForeground(const RGBA& frgba, const RGBA& brgba);

	friend class KTech::Memory;
};
