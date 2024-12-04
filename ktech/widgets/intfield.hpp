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

/*!
	@brief Widget for entering a number.
*/
class IntField : public KTech::Widget
{
public:
	size_t m_number = 0; //!< The entered number adjusted to given maximum and minimum limits.

	std::function<void()> m_OnInsert; //!< Function to call when the user inserts or removes a digit.

	/*!
		@brief Construct an `IntField`.

		@param engine Parent `Engine`.
		@param ui `KTech::UI` to immediately enter.
		@param OnInsert Function to call when the user inserts or removes a digit.
		@param min Minimum number allowed.
		@param max Maximum number allowed.
		@param defaultNum String representing the default number (e.g. `"255"`, `"001"`, or `"1"`; meaningless 0s remain).
		@param position World position.
		@param text Text to the left of the entered number.
		@param withFrame Whether to add a frame around the text and entered number.
		@param unselected Foreground (text and frame) color set when `IntField` is unselected.
		@param selected Foreground color set when `IntField` is selected.
	*/
	IntField(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnInsert,
		size_t min,
		size_t max,
		const std::string& defaultNum = "0",
		KTech::Point position = {0, 0},
		const std::string& text = "Value = ",
		bool withFrame = false,
		KTech::RGBA unselected = KTech::RGBAColors::gray,
		KTech::RGBA selected = KTech::RGBAColors::white)
		: Widget(engine, ui, position), m_OnInsert(std::move(OnInsert)), m_min(min), m_max(max), m_unselectedRGBA(unselected), m_selectedRGBA(selected)
	{
		// Find max allowed digits
		for (size_t i = 1; max / i > 0; i *= 10)
		{
			m_maxDigits++;
		}
		// Find min allowed digits
		for (size_t i = 1; min / i > 0; i *= 10)
		{
			m_minDigits++;
		}
		// Textures
		SetText(text, withFrame);
		// Default value
		SetValue(defaultNum);
		// Input handlers
		m_callbacksGroup.RegisterRangedCallback('0', '9', [this]() -> bool { return Insert(); });
		m_callbacksGroup.RegisterCallback(KTech::Keys::backspace, [this]() -> bool { return Insert(); });
		m_callbacksGroup.RegisterCallback(KTech::Keys::delete_, [this]() -> bool { return Insert(); });
	}

	/*!
		@brief Change the displayed text.

		@param text Text to the left of the entered number.
		@param withFrame Whether to add a frame around the text and entered number.
	*/
	void SetText(const std::string& text, bool withFrame)
	{
		KTech::RGBA tempRGBA = (m_selected ? m_selectedRGBA : m_unselectedRGBA);
		if (withFrame)
		{
			m_textures.resize(TEXTURES_SIZE_FRAMED);
			m_textures[ti_topLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 0));
			m_textures[ti_topRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length() + m_maxDigits, 0));
			m_textures[ti_bottomLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 2));
			m_textures[ti_bottomRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length() + m_maxDigits, 2));
			m_textures[ti_topFrame].Simple(KTech::UPoint(text.length() + m_maxDigits, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 0));
			m_textures[ti_leftFrame].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(0, 1));
			m_textures[ti_bottomFrame].Simple(KTech::UPoint(text.length() + m_maxDigits, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 2));
			m_textures[ti_rightFrame].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(1 + text.length() + m_maxDigits, 1));
		}
		else
		{
			m_textures.resize(TEXTURES_SIZE_FRAMELESS);
		}
		m_textures[ti_input].m_rPos = KTech::Point(1 + text.length(), 1);
		m_textures[ti_text].Write({text}, tempRGBA, KTech::RGBAColors::transparent, KTech::Point(1, 1));
	}

	/*!
		@brief Change the entered number.
		@param number String representing the new number (e.g. `"255"`, `"001"`, or `"1"`; meaningless 0s remain).
	*/
	void SetValue(const std::string& number)
	{
		m_currentDigit = 0;
		m_number = 0;
		KTech::RGBA tempRGBA = (m_selected ? m_selectedRGBA : m_unselectedRGBA);
		m_textures[ti_input].Rectangle(KTech::UPoint(m_maxDigits, 1), KTech::CellA(' ', tempRGBA));
		for (size_t x = 0; x < m_maxDigits && x < number.length(); x++)
		{
			m_currentDigit++;
			m_number *= 10;
			m_number += number[x] - '0';
			m_textures[ti_input](x, 0) = KTech::CellA(number[x], tempRGBA);
		}
		m_visibleNumber = m_number;
	}

private:
	enum TextureIndex : size_t
	{
		ti_input,
		ti_text,
		TEXTURES_SIZE_FRAMELESS,
		ti_topLeftCorner = TEXTURES_SIZE_FRAMELESS,
		ti_topRightCorner,
		ti_bottomLeftCorner,
		ti_bottomRightCorner,
		ti_topFrame,
		ti_leftFrame,
		ti_bottomFrame,
		ti_rightFrame,
		TEXTURES_SIZE_FRAMED
	};

	KTech::RGBA m_unselectedRGBA, m_selectedRGBA;
	size_t m_visibleNumber = 0;
	uint32_t m_min, m_max;
	uint8_t m_maxDigits = 0, m_minDigits = 0;
	uint8_t m_currentDigit = 0;

	void OnSelect() override
	{
		RenderSelected();
	}

	void OnDeselect() override
	{
		RenderUnselected();
	}

	auto Insert() -> bool
	{
		if (engine.input.Is(KTech::Keys::backspace) || engine.input.Is(KTech::Keys::delete_))
		{
			if (m_currentDigit == 0)
			{
				return false;
			}

			m_visibleNumber /= 10;

			m_currentDigit--;
			m_textures[ti_input](m_currentDigit, 0).c = ' ';
		}
		else if (engine.input.Between('0', '9'))
		{
			if (m_currentDigit == m_maxDigits)
			{
				return false;
			}

			m_textures[ti_input](m_currentDigit, 0).c = engine.input.input.at(0);
			m_currentDigit++;

			m_visibleNumber = m_visibleNumber * 10 + engine.input.GetInt();
		}

		if (m_visibleNumber < m_min)
		{
			m_number = m_min;
		}
		else if (m_visibleNumber > m_max)
		{
			m_number = m_max;
		}
		else
		{
			m_number = m_visibleNumber;
		}

		if (m_OnInsert)
		{
			m_OnInsert();
		}
		return true;
	}

	void RenderSelected()
	{
		for (KTech::Texture& texture : m_textures)
		{
			texture.SetForeground(m_selectedRGBA);
		}
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
			m_textures[ti_input].Write({ newTexture }, m_unselectedRGBA, KTech::RGBA(), m_textures[0].m_rPos);
		}
		else if (m_visibleNumber > m_max)
		{
			m_visibleNumber = m_max;

			m_currentDigit = m_maxDigits;

			std::string newTexture = std::to_string(m_max);
			newTexture.resize(m_maxDigits, ' ');
			m_textures[ti_input].Write({ newTexture }, m_unselectedRGBA, KTech::RGBA(), m_textures[0].m_rPos);
		}
		else if (m_visibleNumber >= m_min && m_visibleNumber <= m_max)
		{
			m_number = m_visibleNumber;
		}

		// Change color
		for (KTech::Texture& texture : m_textures)
		{
			texture.SetForeground(m_unselectedRGBA);
		}
	}
};
