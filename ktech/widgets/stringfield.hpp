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
	std::string string = "";

	std::function<void()> OnInsert;

	KTech::RGBA unselectedRGBA, selectedRGBA;

private:
	uint16_t currentChar = 0;
	uint16_t maxChars;

	void InternalInsert()
	{
		if (engine.io.Is(KTech::Keys::backspace) || engine.io.Is(KTech::Keys::delete_))
		{
			if (currentChar == 0)
				return;

			currentChar--;
			string.pop_back();

			textures[0].t[0][currentChar].c = ' ';
		}
		else if (currentChar == maxChars)
			return;
		else
		{
			textures[0].t[0][currentChar].c = engine.io.input.at(0);
			string.push_back(engine.io.input.at(0));
			currentChar++;
		}
		if (OnInsert)
			OnInsert();
	}

public:
	virtual void RenderSelected() override
	{
		for (size_t i = 0; i < textures.size(); i++)
			textures[i].SetForeground(selectedRGBA);
	}

	virtual void RenderUnselected() override
	{
		for (size_t i = 0; i < textures.size(); i++)
			textures[i].SetForeground(unselectedRGBA);
	}

	void ChangeValue(std::string newString)
	{
		string.resize(newString.length() > maxChars ? maxChars : newString.length());
		for (size_t x = 0; x < maxChars; x++)
		{
			if (x < string.length())
			{
				string[x] = newString[x];
				textures[0].t[0][x].c = newString[x];
			}
			else
				textures[0].t[0][x].c = ' ';
		}
		currentChar = string.length();
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
		: Widget(engine, ui, pos), OnInsert(OnInsert), maxChars(maxChars), unselectedRGBA(unselectedRGBA), selectedRGBA(selectedRGBA), string(defaultString)
	{
		// Texture
		if (withFrame)
		{
			textures.resize(10);
			// input
			textures[0].Rectangle(KTech::UPoint(maxChars, 1), KTech::CellA(' ', unselectedRGBA), KTech::Point(1 + text.length(), 1));
			for (size_t i = 0; i < string.length() && i < textures[0].t[0].size(); i++)
			{
				textures[0].t[0][i].c = string[i];
				currentChar++;
			}
			// text
			textures[1].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
			// up-left corner
			textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 0));
			// up-right corner
			textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length() + maxChars, 0));
			// bottom-left corner
			textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 2));
			// bottom-right corner
			textures[5].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length() + maxChars, 2));
			// up frame
			textures[6].Simple(KTech::UPoint(text.length() + maxChars, 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 0));
			// left frame
			textures[7].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(0, 1));
			// bottom frame
			textures[8].Simple(KTech::UPoint(text.length() + maxChars, 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 2));
			// right frame
			textures[9].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(1 + text.length() + maxChars, 1));
		}
		else
		{
			textures.resize(2);
			// input
			textures[0].Rectangle(KTech::UPoint(maxChars, 1), KTech::CellA(' ', unselectedRGBA), KTech::Point(1 + text.length(), 1));
			for (size_t i = 0; i < string.length() && i < textures[0].t[0].size(); i++)
			{
				textures[0].t[0][i].c = string[i];
				currentChar++;
			}
			// text
			textures[1].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
		}
		
		// Input handlers
		for (KeyRange& keyRange : allowedCharacters)
			callbacksGroup->AddCallback(engine.io.RegisterRangedCallback(keyRange.key1, keyRange.key2, std::bind(&StringField::InternalInsert, this)));
		callbacksGroup->AddCallback(engine.io.RegisterCallback(KTech::Keys::delete_, std::bind(&StringField::InternalInsert, this), false));
		callbacksGroup->AddCallback(engine.io.RegisterCallback(KTech::Keys::backspace, std::bind(&StringField::InternalInsert, this), false));
	}
};
