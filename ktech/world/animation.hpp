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

#include "object.hpp"
#include "../engine/engine.hpp"

class KTech::Animation
{
public:
	struct Instruction
	{
		enum class Type : uint8_t
		{
			ParentSetPosition, // Set position of the `Object`
			ParentMove, // Move the `Object` (`Object::Move()`)
			TextureSet, // Hide all other `Texture`s and show the given one
			TextureSetPosition, // Set position of the given `Texture`
			TextureMove, // Add to the position of the given `Texture`
			TextureShow, // Show the given `Texture` (`Texture::visible = true`)
			TextureHide, // Hide the given `Texture` (`Texture::visible = false`)
			Delay
		};

		Type type;
		size_t intData;
		union // In all cases, it may hold only one of these:
		{
			Point pointData;
			Time::Measurement timeMeasurement;
		};

		// TextureSet, TextureSetPosition, TextureShow, TextureHide
		inline Instruction(Type type, size_t textureIndex, Point pointData = Point(0, 0))
			: type(type), intData(textureIndex), pointData(pointData) {}

		// ParentSetPosition, ParentMove
		inline Instruction(Type type, Point pointData)
			: type(type), pointData(pointData) {}

		// Delay
		inline Instruction(Type type, size_t time, Time::Measurement timeMeasurement)
			: type(type), intData(time), timeMeasurement(timeMeasurement) {}
	};

	Engine& engine;

	inline Animation(Engine& engine, ID<Object>& object, const std::vector<Instruction>& instructions)
		: engine(engine), m_object(object), m_instructions(instructions) {}

	virtual ~Animation();

	// Iterates instructions and invokes itself on delay. Will not replay the animation; to do so, call `Animation::Stop()` before calling this again.
	bool Play();
	// Cancel invocations and prepares for a replay.
	void Stop();

protected:
	ID<Object> m_object;
	std::vector<Instruction> m_instructions;
	size_t m_i = 0;
	Time::Invocation* m_invocation = nullptr;
};