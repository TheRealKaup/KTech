#include "widget.hpp"

class IntField : public Widget
{
public:
	uint32_t number = 0;
	uint32_t visibleNumber = 0;

	uint8_t currentDigit = 0;
	
	std::function<void()> OnInsert;

	Engine::RGBA unselectedRGBA, selectedRGBA;

private:
	uint32_t min, max;
	uint8_t maxDigits = 0, minDigits = 0;
	
	void InternalInsert()
	{
		if (Engine::Input::Is(Engine::Input::K::backspace) || Engine::Input::Is(Engine::Input::K::delete_))
		{
			if (currentDigit == 0)
				return;

			visibleNumber /= 10;

			currentDigit--;
			obj.textures[0].t[0][currentDigit].c = ' ';
		}
		else if (Engine::Input::IsNum())
		{
			if (currentDigit == maxDigits)
				return;

			obj.textures[0].t[0][currentDigit].c = Engine::Input::input.at(0);
			currentDigit++;

			visibleNumber = visibleNumber * 10 + Engine::Input::GetNum();
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
	virtual void Select()
	{
		for (size_t i = 0; i < obj.textures.size(); i++)
			obj.textures[i].SetForeground(selectedRGBA);
		selected = true;
		callbackGroup.Enable();
	}

	virtual void Deselect()
	{
		// Correct visible number
		if (visibleNumber < min)
		{
			visibleNumber = min;

			currentDigit = minDigits;

			std::string newTexture = std::to_string(min);
			newTexture.resize(maxDigits, ' ');
			obj.textures[0].Write({ newTexture }, unselectedRGBA, Engine::RGBA(), obj.textures[0].pos_r);
		}
		else if (visibleNumber > max)
		{
			visibleNumber = max;

			currentDigit = maxDigits;

			std::string newTexture = std::to_string(max);
			newTexture.resize(maxDigits, ' ');
			obj.textures[0].Write({ newTexture }, unselectedRGBA, Engine::RGBA(), obj.textures[0].pos_r);
		}
		else if (visibleNumber >= min && visibleNumber <= max)
			number = visibleNumber;

		// Change color
		for (size_t i = 0; i < obj.textures.size(); i++)
			obj.textures[i].SetForeground(unselectedRGBA);

		selected = false;
		callbackGroup.Disable();
	}

	void ChangeValue(std::string newNumber)
	{
		obj.textures[0].Simple(Engine::UPoint( maxDigits, 1 ), Engine::CellA(' ', unselectedRGBA, { 0, 0, 0, 0 }), Engine::Point(1 + (int)obj.textures[1].t[0].size(), 1));
		number = 0;
		currentDigit = 0;
		for (size_t x = 0; x < maxDigits && x < newNumber.length(); x++)
		{
			currentDigit++;
			number *= 10;
			number += newNumber[x] - '0';
			obj.textures[0].t[0][x] = Engine::CellA(newNumber[x], unselectedRGBA);
		}
		visibleNumber = number;
	}

	IntField(Engine::Layer* layer,
		std::function<void()> OnInsert,
		uint32_t min = 0,
		uint32_t max = 255,
		const std::string& defaultNum = "0",
		Engine::Point pos = {0, 0},
		const std::string& text = "Value = ",
		bool withFrame = false,
		Engine::RGBA unselectedRGBA = { 150, 150, 150, 255 },
		Engine::RGBA selectedRGBA = { 255, 255, 255, 255 })
		: OnInsert(OnInsert), min(min), max(max), unselectedRGBA(unselectedRGBA), selectedRGBA(selectedRGBA)
	{
		obj.pos = pos;

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
			obj.textures.resize(10);
			// input
			obj.textures[0].Rectangle(Engine::UPoint(maxDigits, 1), Engine::CellA(' ', unselectedRGBA),  Engine::Point(1 + text.length(), 1));
			for (size_t x = 0; x < maxDigits && x < defaultNum.length(); x++)
			{
				currentDigit++;
				number *= 10;
				number += defaultNum[x] - '0';
				obj.textures[0].t[0][x] = Engine::CellA(defaultNum[x], unselectedRGBA);
			}
			visibleNumber = number;
			// text
			obj.textures[1].Write({text}, unselectedRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
			// up-left corner
			obj.textures[2].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(0, 0));
			// up-right corner
			obj.textures[3].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(1 + text.length() + maxDigits, 0));
			// bottom-left corner
			obj.textures[4].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(0, 2));
			// bottom-right corner
			obj.textures[5].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(1 + text.length() + maxDigits, 2));
			// up frame
			obj.textures[6].Simple(Engine::UPoint(text.length() + maxDigits, 1), Engine::CellA('-', unselectedRGBA), Engine::Point(1, 0));
			// left frame
			obj.textures[7].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedRGBA), Engine::Point(0, 1));
			// bottom frame
			obj.textures[8].Simple(Engine::UPoint(text.length() + maxDigits, 1), Engine::CellA('-', unselectedRGBA), Engine::Point(1, 2));
			// right frame
			obj.textures[9].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedRGBA), Engine::Point(1 + text.length() + maxDigits, 1));
		}
		else
		{
			obj.textures.resize(2);
			// input
			obj.textures[0].Rectangle(Engine::UPoint(maxDigits, 1), Engine::CellA(' ', unselectedRGBA),  Engine::Point(1 + text.length(), 1));
			for (size_t x = 0; x < maxDigits && x < defaultNum.length(); x++)
			{
				currentDigit++;
				number *= 10;
				number += defaultNum[x] - '0';
				obj.textures[0].t[0][x] = Engine::CellA(defaultNum[x], unselectedRGBA);
			}
			visibleNumber = number;
			// text
			obj.textures[1].Write({text}, unselectedRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
		}

		// Input handlers
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("0", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("1", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("2", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("3", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("4", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("5", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("6", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("7", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("8", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback("9", std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback(Engine::Input::K::backspace, std::bind(&IntField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback(Engine::Input::K::delete_, std::bind(&IntField::InternalInsert, this), true));

		// Add object
		layer->AddObject(&obj);
	}
};