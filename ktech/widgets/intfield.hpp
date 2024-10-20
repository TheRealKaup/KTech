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
	
	std::function<void()> m_OnInsert;

	KTech::RGBA m_unselectedRGBA, m_selectedRGBA;

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
		// Find max allowed digits
		for (size_t i = 1; max / i > 0; i *= 10)
			m_maxDigits++;
		// Find min allowed digits
		for (size_t i = 1; min / i > 0; i *= 10)
			m_minDigits++;
		// Textures
		SetText(text, withFrame);
		// Default value
		SetValue(defaultNum);
		// Input handlers
		m_callbacksGroup->AddCallback(engine.input.RegisterRangedCallback('0', '9', std::bind(&IntField::InternalInsert, this)));
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(KTech::Keys::backspace, std::bind(&IntField::InternalInsert, this)));
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(KTech::Keys::delete_, std::bind(&IntField::InternalInsert, this)));
	}

	void SetText(const std::string& text, bool withFrame)
	{
		KTech::RGBA tempRGBA = (m_selected ? m_selectedRGBA : m_unselectedRGBA);
		if (withFrame)
		{
			m_textures.resize(10);
			// up-left corner
			m_textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 0));
			// up-right corner
			m_textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length() + m_maxDigits, 0));
			// bottom-left corner
			m_textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 2));
			// bottom-right corner
			m_textures[5].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length() + m_maxDigits, 2));
			// up frame
			m_textures[6].Simple(KTech::UPoint(text.length() + m_maxDigits, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 0));
			// left frame
			m_textures[7].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(0, 1));
			// bottom frame
			m_textures[8].Simple(KTech::UPoint(text.length() + m_maxDigits, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 2));
			// right frame
			m_textures[9].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(1 + text.length() + m_maxDigits, 1));
		}
		else
			m_textures.resize(2);
		// input position
		m_textures[0].m_rPos = KTech::Point(1 + text.length(), 1);
		// text
		m_textures[1].Write({text}, tempRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
	}

	void SetValue(const std::string& number)
	{
		m_currentDigit = 0;
		m_number = 0;
		KTech::RGBA tempRGBA = (m_selected ? m_selectedRGBA : m_unselectedRGBA);
		m_textures[0].Rectangle(KTech::UPoint(m_maxDigits, 1), KTech::CellA(' ', tempRGBA));
		for (size_t x = 0; x < m_maxDigits && x < number.length(); x++)
		{
			m_currentDigit++;
			m_number *= 10;
			m_number += number[x] - '0';
			m_textures[0](x, 0) = KTech::CellA(number[x], tempRGBA);
		}
		m_visibleNumber = m_number;
	}

protected:
	uint32_t m_min, m_max;
	uint8_t m_maxDigits = 0, m_minDigits = 0;
	uint8_t m_currentDigit = 0;
	
	virtual void OnSelect() override
	{
		RenderSelected();
	}

	virtual void OnDeselect() override
	{
		RenderUnselected();
	}

	bool InternalInsert()
	{
		if (engine.input.Is(KTech::Keys::backspace) || engine.input.Is(KTech::Keys::delete_))
		{
			if (m_currentDigit == 0)
				return false;

			m_visibleNumber /= 10;

			m_currentDigit--;
			m_textures[0](m_currentDigit, 0).c = ' ';
		}
		else if (engine.input.Between('0', '9'))
		{
			if (m_currentDigit == m_maxDigits)
				return false;

			m_textures[0](m_currentDigit, 0).c = engine.input.input.at(0);
			m_currentDigit++;

			m_visibleNumber = m_visibleNumber * 10 + engine.input.GetInt();
		}

		if (m_visibleNumber < m_min)
			m_number = m_min;
		else if (m_visibleNumber > m_max)
			m_number = m_max;
		else
			m_number = m_visibleNumber;

		if (m_OnInsert)
			m_OnInsert();
		return true;
	}

	void RenderSelected()
	{
		for (KTech::Texture& texture : m_textures)
			texture.SetForeground(m_selectedRGBA);
	}

	void RenderUnselected()
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
		for (KTech::Texture& texture : m_textures)
			texture.SetForeground(m_unselectedRGBA);
	}
};
