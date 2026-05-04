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
	@brief Define KTech::Time::Invocation members
	@author Ethan Kaufman (AKA Kaup)
	@date 2023-2026
	@copyright Licensed under GPLv3 or later. Copyright notice in @fileinfo.
*/

#include "invocation.hpp"

#include "../engine/engine.hpp"

KTech::Invocation::Invocation(Engine& p_engine, const std::function<bool()>& p_callback)
	: Entity(p_engine), m_callback(p_callback)
{}

void KTech::Invocation::Invoke(long p_time, Time::Measurement p_measurement)
{
	m_duration = m_engine.time.TimeToMicroseconds(p_time, p_measurement);
	m_timePassed = 0;
	m_active = true;
}

void KTech::Invocation::Cancel()
{
	m_active = false;
	m_duration = 0;
}

auto KTech::Invocation::OnTick() -> bool
{
	if (m_active)
	{
		m_timePassed += m_engine.time.deltaTime;
		if (m_timePassed >= m_duration)
		{
			m_active = false;
			return m_callback();
		}
	}
	return false;
}
