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

#include "animation.hpp"

/*!
	@fn `Animation::Animation`

	@brief Construct an `Animation`, that will not play just yet.

	To start playing the `Animation`, call `Animation::Play()`.

	@param [in] engine Reference to parent engine.
	@param [in] object The object to animate.
	@param [in] instructions Vector of `Animation::Instruction`s, that `Animation::Play()` will interpret and "play".

	@see `Animation::Play()`
*/
KTech::Animation::Animation(Engine& p_engine, const ID<Object>& p_object, const std::vector<Instruction>& p_instructions)
	: engine(p_engine), m_object(p_object), m_instructions(p_instructions), m_invocation(engine, [this]() -> bool { return Play(); }) {}

//! @brief Safely cancels invoked animation instructions.
KTech::Animation::~Animation()
{
	Stop();
}

/*!
	@brief Play the `Animation`.

	Calling this function again does not replay the `Animation`; to do so, call `Animation::Stop()`, and then recall this function.

	This function interprets the instructions, acts upon them, `Time::Invoke()`s itself if there's a delay instruction, and continues interpreting instructions from there.
*/
auto KTech::Animation::Play() -> bool
{
	bool changedThisTick = false; // Render-on-demand
	for (; m_i < m_instructions.size(); m_i++)
	{
		switch (m_instructions[m_i].type)
		{
			case Instruction::Type::ParentSetPosition:
			{
				engine.memory.objects[m_object]->m_pos = m_instructions[m_i].pointData;
				changedThisTick = true;
				break;
			}
			case Instruction::Type::ParentMove:
			{
				engine.memory.objects[m_object]->Move(m_instructions[m_i].pointData);
				changedThisTick = true;
				break;
			}
			case Instruction::Type::TextureSet:
			{
				for (Texture& texture : engine.memory.objects[m_object]->m_textures)
				{
					texture.m_active = false;
				}
				if (m_instructions[m_i].intData < engine.memory.objects[m_object]->m_textures.size())
				{
					engine.memory.objects[m_object]->m_textures[m_instructions[m_i].intData].m_active = true;
				}
				changedThisTick = true;
				break;
			}
			case Instruction::Type::TextureSetPosition:
			{
				if (m_instructions[m_i].intData < engine.memory.objects[m_object]->m_textures.size())
				{
					engine.memory.objects[m_object]->m_textures[m_instructions[m_i].intData].m_rPos = m_instructions[m_i].pointData;
				}
				changedThisTick = true;
				break;
			}
			case Instruction::Type::TextureMove:
			{
				if (m_instructions[m_i].intData < engine.memory.objects[m_object]->m_textures.size())
				{
					engine.memory.objects[m_object]->m_textures[m_instructions[m_i].intData].m_rPos += m_instructions[m_i].pointData;
				}
				changedThisTick = true;
				break;
			}
			case Instruction::Type::TextureShow:
			{
				if (m_instructions[m_i].intData < engine.memory.objects[m_object]->m_textures.size())
				{
					engine.memory.objects[m_object]->m_textures[m_instructions[m_i].intData].m_active = true;
				}
				changedThisTick = true;
				break;
			}
			case Instruction::Type::TextureHide:
			{
				if (m_instructions[m_i].intData < engine.memory.objects[m_object]->m_textures.size())
				{
					engine.memory.objects[m_object]->m_textures[m_instructions[m_i].intData].m_active = false;
				}
				changedThisTick = true;
				break;
			}
			case Instruction::Type::Delay:
			{
				// Delay; invoke the next iteration
				m_invocation.Invoke(m_instructions[m_i].intData, m_instructions[m_i].timeMeasurement);
				// Pause iterating
				m_i++; // Don't reiterate this delay
				return changedThisTick; // Don't set to true as delay changes nothing
			}
		}
	}
	return changedThisTick;
}

//! @brief Stop animation by canceling the invoked instructions, and prepare for a re-`Play()`.
void KTech::Animation::Stop()
{
	m_invocation.Cancel();
	m_i = 0;
}