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

#include "animation.hpp"

KTech::Animation::~Animation()
{
	Stop();
}

bool KTech::Animation::Play()
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
				m_invocation = engine.time.Invoke(std::bind(&Animation::Play, this), m_instructions[m_i].intData, m_instructions[m_i].timeMeasurement);
				// Pause iterating
				m_i++; // Don't reiterate this delay
				return changedThisTick; // Don't set to true as delay changes nothing
			}
		}
	}
	return changedThisTick;
}

// Cancel invocations and prepares for a replay.
void KTech::Animation::Stop()
{
	if (m_invocation != nullptr)
	{
		engine.time.CancelInvocation(m_invocation);
	}
	m_i = 0;
}