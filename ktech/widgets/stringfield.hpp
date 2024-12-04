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
	@brief Widget for entering a string.
*/
class StringField : public KTech::Widget
{
public:
	static constexpr std::pair<char, char> charRange_lower{'a', 'z'}; //!< Range of all lower case letters.
	static constexpr std::pair<char, char> charRange_upper{'A', 'Z'}; //!< Range of all upper case letters.
	static constexpr std::pair<char, char> charRange_numbers{'0', '9'}; //!< Range of all digits.
	static constexpr std::pair<char, char> charRange_all{' ', '~'}; //!< Range ofa all ASCII characters.

	std::string m_string; //!< The entered string.

	std::function<void()> m_OnInsert; //!< Function to call when the user inserts or removes a character.

	/*!
		@brief Construct a `StringField`.

		@param [in] engine Parent `Engine`.
		@param [in] ui `KTech::UI` to immediately enter.
		@param [in] OnInsert Function to call when the user inserts or removes a digit.
		@param [in] allowedCharacters Vector of character pairs, representing the allowed characters (e.g. {StringField::charRange_all}, {{'a', 'c'}, {'x', 'z'}}).
		@param [in] position World position.
		@param [in] text Text to the left of the entered string.
		@param [in] maxChars Maximum allowed characters.
		@param [in] defaultString The default string (e.g. `"abc"`, `"  abc  "`).
		@param [in] withFrame Whether to add a frame around the text and entered string.
		@param [in] unselected Foreground (text and frame) color set when `StringField` is unselected.
		@param [in] selected Foreground color set when `StringField` is selected.
	*/
	StringField(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnInsert = nullptr,
		const std::vector<std::pair<char, char>>& allowedCharacters = {charRange_all},
		KTech::Point position = { 0, 0 },
		const std::string& text = "Value = ",
		unsigned int maxChars = 8,
		const std::string& defaultString = "String",
		bool withFrame = false,
		KTech::RGBA unselected = KTech::RGBAColors::gray,
		KTech::RGBA selected = KTech::RGBAColors::white)
		: Widget(engine, ui, position), m_OnInsert(std::move(OnInsert)), m_maxChars(maxChars), m_unselectedRGBA(unselected), m_selectedRGBA(selected), m_string(defaultString)
	{
		// Textures
		SetText(text, withFrame);
		SetValue(defaultString);
		// Input handlers
		for (const std::pair<char, char>& charRange : allowedCharacters)
		{
			m_callbacksGroup.RegisterRangedCallback(charRange.first, charRange.second, [this]() -> bool { return this->Insert(); });
		}
		m_callbacksGroup.RegisterCallback(KTech::Keys::delete_, [this]() -> bool { return this->Insert(); });
		m_callbacksGroup.RegisterCallback(KTech::Keys::backspace, [this]() -> bool { return this->Insert(); });
	}

	/*!
		@brief Change the displayed text.

		@param [in] text Text to the left of the entered string.
		@param [in] withFrame Whether to add a frame around the text and entered string.
	*/
	void SetText(const std::string& text, bool withFrame)
	{
		KTech::RGBA tempRGBA = (m_selected ? m_selectedRGBA : m_unselectedRGBA);
		if (withFrame)
		{
			m_textures.resize(TEXTURES_SIZE_FRAMED);
			m_textures[ti_topLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 0));
			m_textures[ti_topRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length() + m_maxChars, 0));
			m_textures[ti_bottomLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 2));
			m_textures[ti_bottomRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length() + m_maxChars, 2));
			m_textures[ti_topFrame].Simple(KTech::UPoint(text.length() + m_maxChars, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 0));
			m_textures[ti_leftFrame].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(0, 1));
			m_textures[ti_bottomFrame].Simple(KTech::UPoint(text.length() + m_maxChars, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 2));
			m_textures[ti_rightFrame].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(1 + text.length() + m_maxChars, 1));
		}
		else
		{
			m_textures.resize(TEXTURES_SIZE_FRAMELESS);
		}
		// input position
		m_textures[ti_input].m_rPos = KTech::Point(1 + text.length(), 1);
		// text
		m_textures[ti_text].Write({text}, tempRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
	}

	/*!
		@brief Change the entered string.
		@param [in] value New string (e.g. `"abc"`, `"  abc   "`).
	*/
	void SetValue(const std::string& value)
	{
		m_string = value.substr(0, (m_string.size() > m_maxChars ? m_maxChars : m_string.size()));
		m_textures[ti_input].Write({m_string}, (m_selected ? m_selectedRGBA : m_unselectedRGBA), KTech::RGBAColors::transparent);
		m_currentChar = m_string.length();
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
	uint16_t m_currentChar = 0;
	uint16_t m_maxChars;

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
			if (m_currentChar == 0)
			{
				return false;
			}

			m_currentChar--;
			m_string.pop_back();

			m_textures[ti_input](m_currentChar, 0).c = ' ';
		}
		else if (m_currentChar == m_maxChars)
		{
			return false;
		}
		else
		{
			m_textures[ti_input](m_currentChar, 0).c = engine.input.input.at(0);
			m_string.push_back(engine.input.input.at(0));
			m_currentChar++;
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
		for (KTech::Texture& texture : m_textures)
		{
			texture.SetForeground(m_unselectedRGBA);
		}
	}
};