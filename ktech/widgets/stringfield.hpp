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

struct KeyRange
{
	char key1, key2;
	inline constexpr KeyRange(char key1, char key2) : key1(key1), key2(key2) {}
};

inline constexpr KeyRange keyrange_lower = KeyRange('a', 'z');
inline constexpr KeyRange keyrange_upper = KeyRange('A', 'Z');
inline constexpr KeyRange keyrange_numbers = KeyRange('0', '9');
inline constexpr KeyRange keyrange_all = KeyRange(' ', '~');

class StringField : public KTech::Widget
{
public:
	std::string m_string;

	std::function<void()> m_OnInsert;

	KTech::RGBA m_unselectedRGBA, m_selectedRGBA;

	StringField(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnInsert = nullptr,
		const std::vector<KeyRange>& allowedCharacters = {keyrange_all},
		KTech::Point pos = { 0, 0 },
		const std::string& text = "Value = ",
		unsigned int maxChars = 8,
		const std::string& defaultString = "String",
		bool withFrame = false,
		KTech::RGBA unselectedRGBA = KTech::RGBAColors::gray,
		KTech::RGBA selectedRGBA = KTech::RGBAColors::white)
		: Widget(engine, ui, pos), m_OnInsert(std::move(OnInsert)), m_maxChars(maxChars), m_unselectedRGBA(unselectedRGBA), m_selectedRGBA(selectedRGBA), m_string(defaultString)
	{
		// Textures
		SetText(text, withFrame);
		SetValue(defaultString);
		// Input handlers
		for (const KeyRange& keyRange : allowedCharacters)
		{
			m_callbacksGroup->AddCallback(engine.input.RegisterRangedCallback(keyRange.key1, keyRange.key2, [this]() -> bool { return this->Insert(); }));
		}
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(KTech::Keys::delete_, [this]() -> bool { return this->Insert(); }));
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(KTech::Keys::backspace, [this]() -> bool { return this->Insert(); }));
	}

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

	void SetValue(const std::string& value)
	{
		m_string = value.substr(0, (m_string.size() > m_maxChars ? m_maxChars : m_string.size()));
		m_textures[ti_input].Write({m_string}, (m_selected ? m_selectedRGBA : m_unselectedRGBA), KTech::RGBAColors::transparent);
		m_currentChar = m_string.length();
	}

protected:
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
