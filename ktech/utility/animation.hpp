/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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

#include "../world/object.hpp"
#include "../engine/engine.hpp"
#include "../engine/time/invocation.hpp"

/*!
	@brief Wrapper for animating an `Object`s and its `Texture`s.

	`Animation` contains a vector of `Animation::Instruction`s, which it can play (`Animation::Play()`), stop (`Animation::Stop()`), and play again. This wrapper, which behaves like an instruction interpreter, saves you from writing something bonkers like an `Object`-inherited class that goes through an awkward sequence of `Time::Invoke()`'d member functions to animate its `Texture`s.
*/
class KTech::Animation
{
public:
	//! A single animation instruction. This class is undocumented because it's planned to change (see GitHub issue #126).
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

	Engine& engine; //!< Parent `Engine`

	Animation(Engine& engine, const ID<Object>& object, const std::vector<Instruction>& instructions);
	~Animation();

	auto Play() -> bool;
	void Stop();

private:
	ID<Object> m_object;
	std::vector<Instruction> m_instructions;
	size_t m_i = 0;
	Time::Invocation m_invocation;
};