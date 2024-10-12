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

/*        
	  |  /  ----- ----- ----- -   -  
	 || /\    |   |---  |     |---| 
	|||/  \   |   \____ \____ |   |  kaup's c++ 2d terminal game engine library

	In order to contact me (Kaup), refer to the "readme.md" file.
*/

#pragma once

#include <cstdint>

namespace KTech
{
	// Basic structures.
	// Definitions in `basic_structs/`.
	struct RGB;
	struct RGBA;
	struct Point;
	struct UPoint;
	struct Cell;
	struct CellA;
	// World structures.
	// Definitions in `world/`
	struct Texture;
	struct Collider;
	class Object;
	class Layer;
	class Camera;
	class Map;
	class Widget;
	class UI;
	class Animation;
	// Engine and its components.
	// Definitions in `engine/`
	class Collision;
	class Input;
	class Memory;
	class Output;
	class Time;
	class Engine;
	// Defined in `utility/`
	enum class CR : uint8_t;
	template<class T>
	struct ID;
	template<class T>
	constexpr ID<T> nullID = ID<T>(0, 0);
	template<typename T>
	struct CachingRegistry;
	namespace RGBColors {}
	namespace RGBAColors {}
	namespace Keys {}
}

// Avoids errors caused by files building order by not including the following inclusions in the other header files.
// For example `audio.hpp` might be processed first by the compiler, which will include `engine.hpp` via here, thus causing
// an incomplete type error, as it contains an `Audio` member, meanwhile, the `Audio` class hasn't been defined yet. 
#ifndef KTECH_DEFINITION
#include "basic/point.hpp"
#include "basic/upoint.hpp"
#include "basic/rgb.hpp"
#include "basic/rgba.hpp"
#include "basic/cell.hpp"
#include "basic/cella.hpp"

#include "world/animation.hpp"
#include "world/collider.hpp"
#include "world/texture.hpp"
#include "world/object.hpp"
#include "world/layer.hpp"
#include "world/camera.hpp"
#include "world/map.hpp"
#include "world/widget.hpp"
#include "world/ui.hpp"

#include "utility/container.hpp"
#include "utility/cr.hpp"
#include "utility/id.hpp"
#include "utility/keys.hpp"
#include "utility/rgbcolors.hpp"
#include "utility/rgbacolors.hpp"

#include "engine/collision.hpp"
#include "engine/input/input.hpp"
#include "engine/input/callbacksgroup.hpp"
#include "engine/memory.hpp"
#include "engine/time.hpp"
#include "engine/engine.hpp"
#endif