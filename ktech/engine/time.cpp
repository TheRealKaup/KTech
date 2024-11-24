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

#include "time.hpp"

#include <thread>

auto KTech::Time::Invoke(const std::function<bool()>& p_callback, size_t p_time, Measurement p_timeMeasurement) -> Invocation*
{
	switch (p_timeMeasurement)
	{
		case Measurement::ticks:
		{
			m_invocations.push_back(new Invocation(p_callback, p_time));
			break;
		}
		case Measurement::seconds:
		{
			m_invocations.push_back(new Invocation(p_callback, p_time * tpsLimit));
			break;
		}
		case Measurement::milliseconds:
		{
			m_invocations.push_back(new Invocation(p_callback, p_time * tpsLimit / 1000));
			break;
		}
		case Measurement::microseconds:
		{
			m_invocations.push_back(new Invocation(p_callback, p_time * tpsLimit / 1000000));
			break;
		}
		default:
			return nullptr;
	}
	return m_invocations[m_invocations.size() - 1];
}

auto KTech::Time::CancelInvocation(Invocation* p_invocation) -> bool
{
	for (size_t i = 0; i < m_invocations.size(); i++)
	{
		if (p_invocation == m_invocations[i])
		{
			m_invocations.erase(m_invocations.begin() + i);
			delete p_invocation;
			return true;
		}
	}
	return false;
}


void KTech::Time::CallInvocations()
{
	for (size_t i = 0; i < m_invocations.size(); i++)
	{
		if (m_invocations[i]->ticksLeft > 0)
		{
			m_invocations[i]->ticksLeft--;
		}
		else
		{
			// Call
			if (m_invocations[i]->callback)
			{
				if (m_invocations[i]->callback())
				{
					m_changedThisTick = true;
				}
			}
			// Remove invocation
			delete m_invocations[i];
			m_invocations.erase(m_invocations.begin() + i);
			i--;
			continue;
		}
	}
}

auto KTech::Time::GetDelta(const TimePoint& p_timePointA, const TimePoint& p_timePointB, Measurement p_timeMeasurement) const -> size_t
{
	switch (p_timeMeasurement)
	{
		case Measurement::ticks:
		{
			return std::chrono::duration_cast<std::chrono::seconds>(p_timePointB.chronoTimePoint - p_timePointA.chronoTimePoint).count() * tpsLimit;
		}
		case Measurement::seconds:
		{
			return std::chrono::duration_cast<std::chrono::seconds>(p_timePointB.chronoTimePoint - p_timePointA.chronoTimePoint).count();
		}
		case Measurement::milliseconds:
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(p_timePointB.chronoTimePoint - p_timePointA.chronoTimePoint).count();
		}
		case Measurement::microseconds:
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(p_timePointB.chronoTimePoint - p_timePointA.chronoTimePoint).count();
		}
		default:
			return 0;
	}
}

auto KTech::Time::GetInt(const TimePoint& p_timePoint, Measurement p_timeMeasurement) -> size_t
{
	switch (p_timeMeasurement)
	{
		case Measurement::ticks:
		{
			return std::chrono::duration_cast<std::chrono::seconds>(p_timePoint.chronoTimePoint - m_startTP.chronoTimePoint).count() * tps;
		}
		case Measurement::seconds:
		{
			return std::chrono::duration_cast<std::chrono::seconds>(p_timePoint.chronoTimePoint - m_startTP.chronoTimePoint).count();
		}
		case Measurement::milliseconds:
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(p_timePoint.chronoTimePoint - m_startTP.chronoTimePoint).count();
		}
		case Measurement::microseconds:
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(p_timePoint.chronoTimePoint - m_startTP.chronoTimePoint).count();
		}
		default:
			return 0;
	}
}

void KTech::Time::WaitUntilNextTick()
{
	// Calculate `tpsPotential`
	deltaTime = GetDelta(m_thisTickStartTP, TimePoint() /*now*/, Measurement::microseconds);
	tpsPotential = 1000000.0F / deltaTime;
	// Sleep according to `tpsLimit`
	std::this_thread::sleep_for(std::chrono::microseconds(1000000 / tpsLimit - deltaTime));
	// Calculate (actual) `tps`
	deltaTime = GetDelta(m_thisTickStartTP, TimePoint() /*now*/, Measurement::microseconds);
	tps = 1000000.0F / deltaTime;
	m_thisTickStartTP = TimePoint();
	ticksCounter++;
}