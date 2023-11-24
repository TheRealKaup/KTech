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

class Switch : public Widget
{
public:
	bool down = false;
	bool on = false;

	std::function<void()> OnPress;
	
	Engine::RGBA unselectedOffRGBA, selectedOffRGBA, unselectedOnRGBA, selectedOnRGBA, downRGBA;

private:
	void RemovePressColor()
	{
		if (selected)
		{
			if (on)
				for (size_t i = 0; i < obj.textures.size(); i++)
					obj.textures[i].SetForeground(selectedOnRGBA);
			if (!on)
				for (size_t i = 0; i < obj.textures.size(); i++)
					obj.textures[i].SetForeground(selectedOffRGBA);
		}
		else
		{
			if (on)
				for (size_t i = 0; i < obj.textures.size(); i++)
					obj.textures[i].SetForeground(unselectedOnRGBA);
			if (!on)
				for (size_t i = 0; i < obj.textures.size(); i++)
					obj.textures[i].SetForeground(unselectedOffRGBA);
		}
	}

	void InsideOnPress()
	{
		on = !on;

		for (size_t x = 0; x < obj.textures[0].t[0].size(); x++)
			obj.textures[0].t[0][x].f = downRGBA;
		for (size_t i = 1; i < obj.textures.size(); i++)
			obj.textures[i].value.f = downRGBA;
		
		Engine::Time::Invoke(std::bind(&Switch::RemovePressColor, this), 100, Engine::Time::Measurement::milliseconds);

		if (OnPress)
			OnPress();
	}

public:
	virtual void Select()
	{
		if (on)
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(selectedOnRGBA);
		if (!on)
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(selectedOffRGBA);
		selected = true;
		callbackGroup.Enable();
	}  

	virtual void Deselect()
	{
		if (on)
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(unselectedOnRGBA);
		if (!on)
			for (size_t i = 0; i < obj.textures.size(); i++)
				obj.textures[i].SetForeground(unselectedOffRGBA);
		selected = false;
		callbackGroup.Disable();
	}

	void ChangeValue(bool _on)
	{
		on = _on;
		RemovePressColor();
	}

	Switch(Engine::Layer* layer,
		std::function<void()> OnPress,
		const std::string& key = Engine::Input::K::return_,
		Engine::Point pos = { 0, 0 },
		const std::string& text = "Switch",
		bool on = false,
		bool withFrame = false,
		Engine::RGBA unselectedOffRGBA = Engine::RGBA( 150, 150, 150, 255 ),
		Engine::RGBA selectedOffRGBA = Engine::RGBA( 255, 255, 255, 255 ),
		Engine::RGBA unselectedOnRGBA = Engine::RGBA( 88, 150, 88, 255 ),
		Engine::RGBA selectedOnRGBA = Engine::RGBA( 150, 255, 150, 255 ),
		Engine::RGBA downRGBA = Engine::RGBA(150, 150, 255, 255))
		: OnPress(OnPress), on(on), unselectedOffRGBA(unselectedOffRGBA), selectedOffRGBA(selectedOffRGBA), unselectedOnRGBA(unselectedOnRGBA), selectedOnRGBA(selectedOnRGBA), downRGBA(downRGBA)
	{
		obj.pos = pos;

		// Texture
		if (withFrame)
		{
			if (on)
			{
				obj.textures.resize(9);
				// text
				obj.textures[0].Write({text}, unselectedOnRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
				// up-left corner
				obj.textures[1].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedOnRGBA), Engine::Point(0, 0));
				// up-right corner
				obj.textures[2].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedOnRGBA), Engine::Point(1 + text.length(), 0));
				// bottom-left corner
				obj.textures[3].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedOnRGBA), Engine::Point(0, 2));
				// bottom-right corner
				obj.textures[4].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedOnRGBA), Engine::Point(1 + text.length(), 2));
				// up frame
				obj.textures[5].Simple(Engine::UPoint(text.length(), 1), Engine::CellA('-', unselectedOnRGBA), Engine::Point(1, 0));
				// left frame
				obj.textures[6].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedOnRGBA), Engine::Point(0, 1));
				// bottom frame
				obj.textures[7].Simple(Engine::UPoint(text.length(), 1), Engine::CellA('-', unselectedOnRGBA), Engine::Point(1, 2));
				// right frame
				obj.textures[8].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedOnRGBA), Engine::Point(1 + text.length(), 1));
			}
			else
			{
				obj.textures.resize(9);
				// text
				obj.textures[0].Write({text}, unselectedOffRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
				// up-left corner
				obj.textures[1].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedOffRGBA), Engine::Point(0, 0));
				// up-right corner
				obj.textures[2].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedOffRGBA), Engine::Point(1 + text.length(), 0));
				// bottom-left corner
				obj.textures[3].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedOffRGBA), Engine::Point(0, 2));
				// bottom-right corner
				obj.textures[4].Simple(Engine::UPoint(1, 1), Engine::CellA('#', unselectedOffRGBA), Engine::Point(1 + text.length(), 2));
				// up frame
				obj.textures[5].Simple(Engine::UPoint(text.length(), 1), Engine::CellA('-', unselectedOffRGBA), Engine::Point(1, 0));
				// left frame
				obj.textures[6].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedOffRGBA), Engine::Point(0, 1));
				// bottom frame
				obj.textures[7].Simple(Engine::UPoint(text.length(), 1), Engine::CellA('-', unselectedOffRGBA), Engine::Point(1, 2));
				// right frame
				obj.textures[8].Simple(Engine::UPoint(1, 1), Engine::CellA('|', unselectedOffRGBA), Engine::Point(1 + text.length(), 1));
			}
		}
		else
		{
			if (on)
			{
				obj.textures.resize(1);
				// text
				obj.textures[0].Write({text}, unselectedOnRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
			}
			else
			{
				obj.textures.resize(1);
				// text
				obj.textures[0].Write({text}, unselectedOffRGBA, Engine::RGBA(0, 0, 0, 0), Engine::Point(1, 1));
			}
		}

		// Input handlers
		callbackGroup.AddCallback(Engine::Input::RegisterCallback(key, std::bind(&Switch::InsideOnPress, this), true));
		
		// Add object
		layer->AddObject(&obj);
	}
};