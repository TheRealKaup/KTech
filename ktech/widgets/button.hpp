/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023 E. Kaufman (AKA Kaup)

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

#include "widget.hpp"

class Button : public Widget
{
public:
	std::function<void()> OnPress;
	
	KTech::RGBA unselectedRGBA, selectedRGBA, downRGBA;

private:
	void RemovePressColor()
	{
		if (selected)
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(selectedRGBA);
		else
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(unselectedRGBA);
	}

	void InsideOnPress()
	{
		for (size_t i = 0; i < obj.textures.size(); i++)
			obj.textures[i].SetForeground(downRGBA);
		
		obj.parentLayer->parentMap->parentEngine->time.Invoke(std::bind(&Button::RemovePressColor, this), 100, KTech::Time::Measurement::milliseconds);

		if (OnPress)
			OnPress();
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

	Button(KTech::Layer* layer,
		std::function<void()> OnPress,
		std::string key = KTech::Keys::return_,
		KTech::Point pos = { 0, 0 },
		const std::string& text = "Button",
		bool withFrame = false,
		KTech::RGBA unselectedRGBA = KTech::RGBA( 150, 150, 150, 255 ),
		KTech::RGBA selectedRGBA = KTech::RGBA( 255, 255, 255, 255 ),
		KTech::RGBA downRGBA = KTech::RGBA(150, 150, 255, 255))
		: OnPress(OnPress), unselectedRGBA(unselectedRGBA), selectedRGBA(selectedRGBA), downRGBA(downRGBA)
	{
		obj.pos = pos;

		// Texture
		if (withFrame)
		{	
			obj.textures.resize(9);
			// text
			obj.textures[0].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
			// up-left corner
			obj.textures[1].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 0));
			// up-right corner
			obj.textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length(), 0));
			// bottom-left corner
			obj.textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 2));
			// bottom-right corner
			obj.textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length(), 2));
			// up frame
			obj.textures[5].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 0));
			// left frame
			obj.textures[6].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(0, 1));
			// bottom frame
			obj.textures[7].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 2));
			// right frame
			obj.textures[8].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(1 + text.length(), 1));
		}
		else
		{
			obj.textures.resize(1);
			// text
			obj.textures[0].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
		}

		// Input handlers
		callbackGroup.AddCallback(layer->parentMap->parentEngine->io.RegisterCallback(key, std::bind(&Button::InsideOnPress, this), true));

		// Add object
		layer->AddObject(&obj);
	}
};
