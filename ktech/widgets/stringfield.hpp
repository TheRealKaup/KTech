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
	std::string m_string = "";

	std::function<void()> m_OnInsert;

	KTech::RGBA m_unselectedRGBA, m_selectedRGBA;

private:
	uint16_t m_currentChar = 0;
	uint16_t m_maxChars;

	void InternalInsert()
	{
		if (engine.input.Is(KTech::Keys::backspace) || engine.input.Is(KTech::Keys::delete_))
		{
			if (m_currentChar == 0)
				return;

			m_currentChar--;
			m_string.pop_back();

			m_textures[0].m_t[0][m_currentChar].c = ' ';
		}
		else if (m_currentChar == m_maxChars)
			return;
		else
		{
			m_textures[0].m_t[0][m_currentChar].c = engine.input.input.at(0);
			m_string.push_back(engine.input.input.at(0));
			m_currentChar++;
		}
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
		for (size_t i = 0; i < m_textures.size(); i++)
			m_textures[i].SetForeground(m_unselectedRGBA);
	}

	void ChangeValue(std::string newString)
	{
		m_string.resize(newString.length() > m_maxChars ? m_maxChars : newString.length());
		for (size_t x = 0; x < m_maxChars; x++)
		{
			if (x < m_string.length())
			{
				m_string[x] = newString[x];
				m_textures[0].m_t[0][x].c = newString[x];
			}
			else
				m_textures[0].m_t[0][x].c = ' ';
		}
		m_currentChar = m_string.length();
	}

	StringField(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnInsert = 0,
		std::vector<KeyRange> allowedCharacters = {keyrange_all},
		KTech::Point pos = { 0, 0 },
		const std::string& text = "Value = ",
		unsigned int maxChars = 8,
		const std::string& defaultString = "String",
		bool withFrame = false,
		KTech::RGBA unselectedRGBA = KTech::RGBA(150, 150, 150, 255),
		KTech::RGBA selectedRGBA = KTech::RGBAColors::white)
		: Widget(engine, ui, pos), m_OnInsert(OnInsert), m_maxChars(maxChars), m_unselectedRGBA(unselectedRGBA), m_selectedRGBA(selectedRGBA), m_string(defaultString)
	{
		// Texture
		if (withFrame)
		{
			m_textures.resize(10);
			// input
			m_textures[0].Rectangle(KTech::UPoint(maxChars, 1), KTech::CellA(' ', unselectedRGBA), KTech::Point(1 + text.length(), 1));
			for (size_t i = 0; i < m_string.length() && i < m_textures[0].m_t[0].size(); i++)
			{
				m_textures[0].m_t[0][i].c = m_string[i];
				m_currentChar++;
			}
			// text
			m_textures[1].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
			// up-left corner
			m_textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 0));
			// up-right corner
			m_textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length() + maxChars, 0));
			// bottom-left corner
			m_textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 2));
			// bottom-right corner
			m_textures[5].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length() + maxChars, 2));
			// up frame
			m_textures[6].Simple(KTech::UPoint(text.length() + maxChars, 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 0));
			// left frame
			m_textures[7].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(0, 1));
			// bottom frame
			m_textures[8].Simple(KTech::UPoint(text.length() + maxChars, 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 2));
			// right frame
			m_textures[9].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(1 + text.length() + maxChars, 1));
		}
		else
		{
			m_textures.resize(2);
			// input
			m_textures[0].Rectangle(KTech::UPoint(maxChars, 1), KTech::CellA(' ', unselectedRGBA), KTech::Point(1 + text.length(), 1));
			for (size_t i = 0; i < m_string.length() && i < m_textures[0].m_t[0].size(); i++)
			{
				m_textures[0].m_t[0][i].c = m_string[i];
				m_currentChar++;
			}
			// text
			m_textures[1].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
		}
		
		// Input handlers
		for (KeyRange& keyRange : allowedCharacters)
			m_callbacksGroup->AddCallback(engine.input.RegisterRangedCallback(keyRange.key1, keyRange.key2, std::bind(&StringField::InternalInsert, this)));
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(KTech::Keys::delete_, std::bind(&StringField::InternalInsert, this), false));
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(KTech::Keys::backspace, std::bind(&StringField::InternalInsert, this), false));
	}
};
