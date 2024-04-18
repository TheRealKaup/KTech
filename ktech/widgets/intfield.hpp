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

#include "../ktech.hpp"

class IntField : public KTech::Widget
{
public:
	uint32_t m_number = 0;
	uint32_t m_visibleNumber = 0;

	uint8_t m_currentDigit = 0;
	
	std::function<void()> m_OnInsert;

	KTech::RGBA m_unselectedRGBA, m_selectedRGBA;

private:
	uint32_t m_min, m_max;
	uint8_t m_maxDigits = 0, m_minDigits = 0;
	
	void InternalInsert()
	{
		if (engine.input.Is(KTech::Keys::backspace) || engine.input.Is(KTech::Keys::delete_))
		{
			if (m_currentDigit == 0)
				return;

			m_visibleNumber /= 10;

			m_currentDigit--;
			m_textures[0].m_t[0][m_currentDigit].c = ' ';
		}
		else if (engine.input.Between('0', '9'))
		{
			if (m_currentDigit == m_maxDigits)
				return;

			m_textures[0].m_t[0][m_currentDigit].c = engine.input.input.at(0);
			m_currentDigit++;

			m_visibleNumber = m_visibleNumber * 10 + engine.input.GetInt();
		}

		m_number = m_visibleNumber;
		if (m_number < m_min)
			m_number = m_min;
		if (m_number > m_max)
			m_number = m_max;

		if (m_OnInsert)
			m_OnInsert();
	}

public:
	virtual void RenderSelected() override
	{
		for (size_t i = 0; i < m_textures.size(); i++)
			m_textures[i].SetForeground(m_selectedRGBA);
	}

	virtual void RenderUnselected() override
	{
		// Correct visible number
		if (m_visibleNumber < m_min)
		{
			m_visibleNumber = m_min;

			m_currentDigit = m_minDigits;

			std::string newTexture = std::to_string(m_min);
			newTexture.resize(m_maxDigits, ' ');
			m_textures[0].Write({ newTexture }, m_unselectedRGBA, KTech::RGBA(), m_textures[0].m_rPos);
		}
		else if (m_visibleNumber > m_max)
		{
			m_visibleNumber = m_max;

			m_currentDigit = m_maxDigits;

			std::string newTexture = std::to_string(m_max);
			newTexture.resize(m_maxDigits, ' ');
			m_textures[0].Write({ newTexture }, m_unselectedRGBA, KTech::RGBA(), m_textures[0].m_rPos);
		}
		else if (m_visibleNumber >= m_min && m_visibleNumber <= m_max)
			m_number = m_visibleNumber;

		// Change color
		for (size_t i = 0; i < m_textures.size(); i++)
			m_textures[i].SetForeground(m_unselectedRGBA);
	}

	void ChangeValue(std::string newNumber)
	{
		m_textures[0].Simple(KTech::UPoint( m_maxDigits, 1 ), KTech::CellA(' ', m_unselectedRGBA, { 0, 0, 0, 0 }), KTech::Point(1 + (int)m_textures[1].m_t[0].size(), 1));
		m_number = 0;
		m_currentDigit = 0;
		for (size_t x = 0; x < m_maxDigits && x < newNumber.length(); x++)
		{
			m_currentDigit++;
			m_number *= 10;
			m_number += newNumber[x] - '0';
			m_textures[0].m_t[0][x] = KTech::CellA(newNumber[x], m_unselectedRGBA);
		}
		m_visibleNumber = m_number;
	}

	IntField(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnInsert,
		uint32_t min = 0,
		uint32_t max = 255,
		const std::string& defaultNum = "0",
		KTech::Point pos = {0, 0},
		const std::string& text = "Value = ",
		bool withFrame = false,
		KTech::RGBA unselectedRGBA = { 150, 150, 150, 255 },
		KTech::RGBA selectedRGBA = { 255, 255, 255, 255 })
		: Widget(engine, ui, pos), m_OnInsert(OnInsert), m_min(min), m_max(max), m_unselectedRGBA(unselectedRGBA), m_selectedRGBA(selectedRGBA)
	{
		for (size_t i = 1; true; i *= 10)
		{
			if (max / i > 0)
				m_maxDigits++;
			else
				break;
		}
		for (size_t i = 1; true; i *= 10)
		{
			if (min / i > 0)
				m_minDigits++;
			else
				break;
		}

		// Texture
		if (withFrame)
		{
			m_textures.resize(10);
			// input
			m_textures[0].Rectangle(KTech::UPoint(m_maxDigits, 1), KTech::CellA(' ', unselectedRGBA),  KTech::Point(1 + text.length(), 1));
			for (size_t x = 0; x < m_maxDigits && x < defaultNum.length(); x++)
			{
				m_currentDigit++;
				m_number *= 10;
				m_number += defaultNum[x] - '0';
				m_textures[0].m_t[0][x] = KTech::CellA(defaultNum[x], unselectedRGBA);
			}
			m_visibleNumber = m_number;
			// text
			m_textures[1].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
			// up-left corner
			m_textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 0));
			// up-right corner
			m_textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length() + m_maxDigits, 0));
			// bottom-left corner
			m_textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 2));
			// bottom-right corner
			m_textures[5].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length() + m_maxDigits, 2));
			// up frame
			m_textures[6].Simple(KTech::UPoint(text.length() + m_maxDigits, 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 0));
			// left frame
			m_textures[7].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(0, 1));
			// bottom frame
			m_textures[8].Simple(KTech::UPoint(text.length() + m_maxDigits, 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 2));
			// right frame
			m_textures[9].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(1 + text.length() + m_maxDigits, 1));
		}
		else
		{
			m_textures.resize(2);
			// input
			m_textures[0].Rectangle(KTech::UPoint(m_maxDigits, 1), KTech::CellA(' ', unselectedRGBA),  KTech::Point(1 + text.length(), 1));
			for (size_t x = 0; x < m_maxDigits && x < defaultNum.length(); x++)
			{
				m_currentDigit++;
				m_number *= 10;
				m_number += defaultNum[x] - '0';
				m_textures[0].m_t[0][x] = KTech::CellA(defaultNum[x], unselectedRGBA);
			}
			m_visibleNumber = m_number;
			// text
			m_textures[1].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
		}

		// Input handlers
		m_callbacksGroup->AddCallback(engine.input.RegisterRangedCallback('0', '9', std::bind(&IntField::InternalInsert, this)));
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(KTech::Keys::backspace, std::bind(&IntField::InternalInsert, this), false));
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(KTech::Keys::delete_, std::bind(&IntField::InternalInsert, this), false));
	}
};
