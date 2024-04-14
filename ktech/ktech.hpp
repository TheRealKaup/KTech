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
	// Definitions in `world_structs/`
	struct Texture;
	struct Collider;
	struct Object;
	struct Layer;
	struct Camera;
	struct Map;
	struct Widget;
	struct UI;
	// Engine and its components.
	// Definitions in `engine/`
	struct IO;
	class Audio;
	class Collision;
	class Memory;
	class Time;
	class Engine;
	// Defined in `misc/`
	enum class CR : uint8_t;	
	template<class T>
	struct ID;
	template<typename T>
	struct Container;
	namespace RGBColors {}
	namespace RGBAColors {}
	namespace Keys {}
}

#ifndef KTECH_DEFINITION
#ifndef KTECH_H
#define KTECH_H
#include "basic_structs/point.hpp"
#include "basic_structs/upoint.hpp"
#include "basic_structs/rgb.hpp"
#include "basic_structs/rgba.hpp"
#include "basic_structs/cell.hpp"
#include "basic_structs/cella.hpp"

#include "world_structs/collider.hpp"
#include "world_structs/texture.hpp"
#include "world_structs/object.hpp"
#include "world_structs/layer.hpp"
#include "world_structs/camera.hpp"
#include "world_structs/map.hpp"
#include "world_structs/widget.hpp"
#include "world_structs/ui.hpp"

#include "misc/container.hpp"
#include "misc/cr.hpp"
#include "misc/id.hpp"
#include "misc/keys.hpp"
#include "misc/rgbcolors.hpp"
#include "misc/rgbacolors.hpp"

#include "engine/audio.hpp"
#include "engine/collision.hpp"
#include "engine/io.hpp"
#include "engine/memory.hpp"
#include "engine/time.hpp"
#include "engine/engine.hpp"
#endif
#endif