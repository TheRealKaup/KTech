#include "widget.hpp"

struct KeyRange
{
	char key1, key2;
	inline constexpr KeyRange(char key1, char key2) : key1(key1), key2(key2) {}
};

inline constexpr KeyRange keyrange_lower = KeyRange('a', 'z');
inline constexpr KeyRange keyrange_upper = KeyRange('A', 'Z');
inline constexpr KeyRange keyrange_numbers = KeyRange('0', '9');
inline constexpr KeyRange keyrange_all = KeyRange(' ', '~');

class StringField : public Widget
{
public:
	std::string string = "";

	std::function<void()> OnInsert;

	Engine::RGBA unselectedRGBA, selectedRGBA;

private:
	uint16_t currentChar = 0;
	uint16_t maxChars;

	void InternalInsert()
	{
		std::cerr << Engine::Input::input << std::endl; 
		if (Engine::Input::Is(Engine::Input::K::backspace) || Engine::Input::Is(Engine::Input::K::delete_))
		{
			if (currentChar == 0)
				return;

			currentChar--;
			string.pop_back();

			obj.textures[0].t[0][currentChar].c = ' ';
		}
		else if (currentChar == maxChars)
			return;
		else
		{
			obj.textures[0].t[0][currentChar].c = Engine::Input::input.at(0);
			string.push_back(Engine::Input::input.at(0));
			currentChar++;
		}
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
		for (size_t i = 0; i < obj.textures.size(); i++)
			obj.textures[i].SetForeground(unselectedRGBA);
		selected = false;
		callbackGroup.Disable();
	}

	void ChangeValue(std::string newString)
	{
		string.resize(newString.length() > maxChars ? maxChars : newString.length());
		for (size_t x = 0; x < maxChars; x++)
		{
			if (x < string.length())
			{
				string[x] = newString[x];
				obj.textures[0].t[0][x].c = newString[x];
			}
			else
				obj.textures[0].t[0][x].c = ' ';
		}
		currentChar = string.length();
	}

	StringField(Engine::Layer* layer = 0,
		std::function<void()> OnInsert = 0,
		std::vector<KeyRange> allowedCharacters = {keyrange_all},
		Engine::Point pos = { 0, 0 },
		const std::string& text = "Value = ",
		unsigned int maxChars = 8,
		const std::string& defaultString = "String",
		bool withFrame = false,
		Engine::RGBA unselectedRGBA = Engine::RGBA(150, 150, 150, 255),
		Engine::RGBA selectedRGBA = Engine::RGBAColors::white)
		: OnInsert(OnInsert), maxChars(maxChars), unselectedRGBA(unselectedRGBA), selectedRGBA(selectedRGBA), string(defaultString)
	{
		obj.pos = pos;

		// Texture
		if (withFrame)
		{
			obj.textures.resize(10);
			// input
			obj.textures[0].Rectangle(Engine::UPoint(maxChars, 1), Engine::CellA(' ', unselectedRGBA), Engine::Point(1 + text.length(), 1));
			for (size_t i = 0; i < string.length() && i < obj.textures[0].t[0].size(); i++)
			{
				obj.textures[0].t[0][i].c = string[i];
				currentChar++;
			}
			// text
			obj.textures[1].Write({text}, unselectedRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
			// up-left corner
			obj.textures[2].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(0, 0));
			// up-right corner
			obj.textures[3].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(1 + text.length() + maxChars, 0));
			// bottom-left corner
			obj.textures[4].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(0, 2));
			// bottom-right corner
			obj.textures[5].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedRGBA), Engine::Point(1 + text.length() + maxChars, 2));
			// up frame
			obj.textures[6].Simple(Engine::UPoint(text.length() + maxChars, 1), Engine::CellA('-', unselectedRGBA), Engine::Point(1, 0));
			// left frame
			obj.textures[7].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedRGBA), Engine::Point(0, 1));
			// bottom frame
			obj.textures[8].Simple(Engine::UPoint(text.length() + maxChars, 1), Engine::CellA('-', unselectedRGBA), Engine::Point(1, 2));
			// right frame
			obj.textures[9].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedRGBA), Engine::Point(1 + text.length() + maxChars, 1));
		}
		else
		{
			obj.textures.resize(2);
			// input
			obj.textures[0].Rectangle(Engine::UPoint(maxChars, 1), Engine::CellA(' ', unselectedRGBA), Engine::Point(1 + text.length(), 1));
			for (size_t i = 0; i < string.length() && i < obj.textures[0].t[0].size(); i++)
			{
				obj.textures[0].t[0][i].c = string[i];
				currentChar++;
			}
			// text
			obj.textures[1].Write({text}, unselectedRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
		}
		
		// Input handlers
		for (KeyRange& keyRange : allowedCharacters)
			callbackGroup.AddCallback(Engine::Input::RegisterRangedCallback(keyRange.key1, keyRange.key2, std::bind(&StringField::InternalInsert, this)));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback(Engine::Input::K::delete_, std::bind(&StringField::InternalInsert, this), true));
		callbackGroup.AddCallback(Engine::Input::RegisterCallback(Engine::Input::K::backspace, std::bind(&StringField::InternalInsert, this), true));
		
		// Add object
		layer->AddObject(&obj);
	}
};