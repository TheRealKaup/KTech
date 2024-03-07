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

#include "../ktech.hpp"

class IntField : public KTech::Widget
{
public:
	uint32_t number = 0;
	uint32_t visibleNumber = 0;

	uint8_t currentDigit = 0;
	
	std::function<void()> OnInsert;

	KTech::RGBA unselectedRGBA, selectedRGBA;

private:
	uint32_t min, max;
	uint8_t maxDigits = 0, minDigits = 0;
	
	void InternalInsert()
	{
		if (engine.io.Is(KTech::Keys::backspace) || engine.io.Is(KTech::Keys::delete_))
		{
			if (currentDigit == 0)
				return;

			visibleNumber /= 10;

			currentDigit--;
			textures[0].t[0][currentDigit].c = ' ';
		}
		else if (engine.io.Between('0', '9'))
		{
			if (currentDigit == maxDigits)
				return;

			textures[0].t[0][currentDigit].c = engine.io.input.at(0);
			currentDigit++;

			visibleNumber = visibleNumber * 10 + engine.io.GetInt();
		}

		number = visibleNumber;
		if (number < min)
			number = min;
		if (number > max)
			number = max;

		if (OnInsert)
			OnInsert();
	}

public:
	virtual void RenderSelected()
	{
		for (size_t i = 0; i < textures.size(); i++)
			textures[i].SetForeground(selectedRGBA);
	}

	virtual void RenderUnselected()
	{
		// Correct visible number
		if (visibleNumber < min)
		{
			visibleNumber = min;

			currentDigit = minDigits;

			std::string newTexture = std::to_string(min);
			newTexture.resize(maxDigits, ' ');
			textures[0].Write({ newTexture }, unselectedRGBA, KTech::RGBA(), textures[0].pos_r);
		}
		else if (visibleNumber > max)
		{
			visibleNumber = max;

			currentDigit = maxDigits;

			std::string newTexture = std::to_string(max);
			newTexture.resize(maxDigits, ' ');
			textures[0].Write({ newTexture }, unselectedRGBA, KTech::RGBA(), textures[0].pos_r);
		}
		else if (visibleNumber >= min && visibleNumber <= max)
			number = visibleNumber;

		// Change color
		for (size_t i = 0; i < textures.size(); i++)
			textures[i].SetForeground(unselectedRGBA);
	}

	void ChangeValue(std::string newNumber)
	{
		textures[0].Simple(KTech::UPoint( maxDigits, 1 ), KTech::CellA(' ', unselectedRGBA, { 0, 0, 0, 0 }), KTech::Point(1 + (int)textures[1].t[0].size(), 1));
		number = 0;
		currentDigit = 0;
		for (size_t x = 0; x < maxDigits && x < newNumber.length(); x++)
		{
			currentDigit++;
			number *= 10;
			number += newNumber[x] - '0';
			textures[0].t[0][x] = KTech::CellA(newNumber[x], unselectedRGBA);
		}
		visibleNumber = number;
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
		: Widget(engine, ui, pos), OnInsert(OnInsert), min(min), max(max), unselectedRGBA(unselectedRGBA), selectedRGBA(selectedRGBA)
	{
		for (size_t i = 1; true; i *= 10)
		{
			if (max / i > 0)
				maxDigits++;
			else
				break;
		}
		for (size_t i = 1; true; i *= 10)
		{
			if (min / i > 0)
				minDigits++;
			else
				break;
		}

		// Texture
		if (withFrame)
		{
			textures.resize(10);
			// input
			textures[0].Rectangle(KTech::UPoint(maxDigits, 1), KTech::CellA(' ', unselectedRGBA),  KTech::Point(1 + text.length(), 1));
			for (size_t x = 0; x < maxDigits && x < defaultNum.length(); x++)
			{
				currentDigit++;
				number *= 10;
				number += defaultNum[x] - '0';
				textures[0].t[0][x] = KTech::CellA(defaultNum[x], unselectedRGBA);
			}
			visibleNumber = number;
			// text
			textures[1].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
			// up-left corner
			textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 0));
			// up-right corner
			textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length() + maxDigits, 0));
			// bottom-left corner
			textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 2));
			// bottom-right corner
			textures[5].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length() + maxDigits, 2));
			// up frame
			textures[6].Simple(KTech::UPoint(text.length() + maxDigits, 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 0));
			// left frame
			textures[7].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(0, 1));
			// bottom frame
			textures[8].Simple(KTech::UPoint(text.length() + maxDigits, 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 2));
			// right frame
			textures[9].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(1 + text.length() + maxDigits, 1));
		}
		else
		{
			textures.resize(2);
			// input
			textures[0].Rectangle(KTech::UPoint(maxDigits, 1), KTech::CellA(' ', unselectedRGBA),  KTech::Point(1 + text.length(), 1));
			for (size_t x = 0; x < maxDigits && x < defaultNum.length(); x++)
			{
				currentDigit++;
				number *= 10;
				number += defaultNum[x] - '0';
				textures[0].t[0][x] = KTech::CellA(defaultNum[x], unselectedRGBA);
			}
			visibleNumber = number;
			// text
			textures[1].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
		}

		// Input handlers
		callbacksGroup->AddCallback(engine.io.RegisterRangedCallback('0', '9', std::bind(&IntField::InternalInsert, this)));
		callbacksGroup->AddCallback(engine.io.RegisterCallback(KTech::Keys::backspace, std::bind(&IntField::InternalInsert, this), false));
		callbacksGroup->AddCallback(engine.io.RegisterCallback(KTech::Keys::delete_, std::bind(&IntField::InternalInsert, this), false));
	}
};
