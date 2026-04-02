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

/*!
	@file
	@brief Define KTech::Animation class
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#pragma once

#define KTECH_DEFINITION
#include "../ktech.hpp"
#undef KTECH_DEFINITION

#include "../engine/engine.hpp"
#include "../engine/time/invocation.hpp"
#include "../world/object.hpp"

/*!
	@brief Wrapper for animating an `Object` and its `Texture`s.

	`Animation` contains a vector of `Animation::Instruction`s, which it can play (`Animation::Play()`), stop (`Animation::Stop()`), and play again. This wrapper, which behaves like an instruction interpreter, saves you from writing something bonkers like an `Object`-inherited class that goes through an awkward sequence of `Time::Invoke()`'d member functions to animate its `Texture`s.
*/
class KTech::Animation
{
public:
	/*!
		@brief A single animation instruction

		@deprecated See GitHub issue #126
	*/
	struct Instruction
	{
		//! @deprecated See GitHub issue #126
		enum class Type : uint8_t
		{
			ParentSetPosition,	//!< Set position of the `Object`
			ParentMove,			//!< Move the `Object` (`Object::Move()`)
			TextureSet,			//!< Hide all other `Texture`s and show the given one
			TextureSetPosition, //!< Set position of the given `Texture`
			TextureMove,		//!< Add to the position of the given `Texture`
			TextureShow,		//!< Show the given `Texture` (`Texture::visible = true`)
			TextureHide,		//!< Hide the given `Texture` (`Texture::visible = false`)
			Delay				//!< Wait ticks before next instruction
		};

		//! @deprecated See GitHub issue #126
		Type type;
		//! @deprecated See GitHub issue #126
		size_t intData;
		union // In all cases, it may hold only one of these:
		{
			Point pointData;
			Time::Measurement timeMeasurement;
		};

		/*!
			@brief TextureSet, TextureSetPosition, TextureShow, TextureHide

			@deprecated See GitHub issue #126

			@param type
			@param textureIndex
			@param pointData
		*/
		Instruction(Type type, size_t textureIndex, Point pointData = Point(0, 0))
			: type(type), intData(textureIndex), pointData(pointData)
		{}

		/*!
			@brief ParentSetPosition, ParentMove

			@deprecated See GitHub issue #126

			@param type
			@param pointData
		*/
		Instruction(Type type, Point pointData)
			: type(type), pointData(pointData)
		{}

		/*!
			@brief Delay

			@deprecated See GitHub issue #126

			@param type
			@param time
			@param timeMeasurement
		*/
		Instruction(Type type, size_t time, Time::Measurement timeMeasurement)
			: type(type), intData(time), timeMeasurement(timeMeasurement)
		{}
	};

	Engine& engine; //!< Parent `Engine`

	/*!
		@fn `Animation::Animation`

		@brief Construct an `Animation`, that will not play just yet.

		To start playing the `Animation`, call `Animation::Play()`.

		@param [in] engine Reference to parent engine.
		@param [in] object The object to animate.
		@param [in] instructions Vector of `Animation::Instruction`s, that `Animation::Play()` will interpret and "play".

		@see `Animation::Play()`
	*/
	Animation(Engine& engine, const ID<Object>& object, const std::vector<Instruction>& instructions);

	/*!
		@brief Safely cancels invoked animation instructions.
	*/
	~Animation();

	/*!
		@brief Play the `Animation`.

		Calling this function again does not replay the `Animation`; to do so, call `Animation::Stop()`, and then recall this function.

		This function interprets the instructions, acts upon them, `Time::Invoke()`s itself if there's a delay instruction, and continues interpreting instructions from there.

		@return `true` if changed something and thus should render this tick, `false` otherwise.
	*/
	auto Play() -> bool;

	/*!
		@brief Stop animation by canceling the invoked instructions, and prepare for a re-`Play()`.
	*/
	void Stop();

private:
	ID<Object> m_object;
	std::vector<Instruction> m_instructions;
	size_t m_i = 0;
	Time::Invocation m_invocation;
};
