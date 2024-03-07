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

class Switch : public KTech::Widget
{
public:
	bool down = false;
	bool on = false;

	std::function<void()> OnPress;
	
	KTech::RGBA unselectedOffRGBA, selectedOffRGBA, unselectedOnRGBA, selectedOnRGBA, downRGBA;

private:
	void RemovePressColor()
	{
		if (selected)
		{
			if (on)
				for (size_t i = 0; i < textures.size(); i++)
					textures[i].SetForeground(selectedOnRGBA);
			if (!on)
				for (size_t i = 0; i < textures.size(); i++)
					textures[i].SetForeground(selectedOffRGBA);
		}
		else
		{
			if (on)
				for (size_t i = 0; i < textures.size(); i++)
					textures[i].SetForeground(unselectedOnRGBA);
			if (!on)
				for (size_t i = 0; i < textures.size(); i++)
					textures[i].SetForeground(unselectedOffRGBA);
		}
	}

	void InsideOnPress()
	{
		on = !on;

		for (size_t x = 0; x < textures[0].t[0].size(); x++)
			textures[0].t[0][x].f = downRGBA;
		for (size_t i = 1; i < textures.size(); i++)
			textures[i].value.f = downRGBA;
		
		engine.time.Invoke(std::bind(&Switch::RemovePressColor, this), 100, KTech::Time::Measurement::milliseconds);

		if (OnPress)
			OnPress();
	}

public:
	virtual void RenderSelected()
	{
		if (on)
			for (size_t i = 0; i < textures.size(); i++)
				textures[i].SetForeground(selectedOnRGBA);
		if (!on)
			for (size_t i = 0; i < textures.size(); i++)
				textures[i].SetForeground(selectedOffRGBA);
	}  

	virtual void RenderUnselected()
	{
		if (on)
			for (size_t i = 0; i < textures.size(); i++)
				textures[i].SetForeground(unselectedOnRGBA);
		if (!on)
			for (size_t i = 0; i < textures.size(); i++)
				textures[i].SetForeground(unselectedOffRGBA);
	}

	void ChangeValue(bool _on)
	{
		on = _on;
		RemovePressColor();
	}

	Switch(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnPress,
		const std::string& key = KTech::Keys::return_,
		KTech::Point pos = { 0, 0 },
		const std::string& text = "Switch",
		bool on = false,
		bool withFrame = false,
		KTech::RGBA unselectedOffRGBA = KTech::RGBA( 150, 150, 150, 255 ),
		KTech::RGBA selectedOffRGBA = KTech::RGBA( 255, 255, 255, 255 ),
		KTech::RGBA unselectedOnRGBA = KTech::RGBA( 88, 150, 88, 255 ),
		KTech::RGBA selectedOnRGBA = KTech::RGBA( 150, 255, 150, 255 ),
		KTech::RGBA downRGBA = KTech::RGBA(150, 150, 255, 255))
		: Widget(engine, ui, pos), OnPress(OnPress), on(on), unselectedOffRGBA(unselectedOffRGBA), selectedOffRGBA(selectedOffRGBA), unselectedOnRGBA(unselectedOnRGBA), selectedOnRGBA(selectedOnRGBA), downRGBA(downRGBA)
	{
		// Texture
		if (withFrame)
		{
			if (on)
			{
				textures.resize(9);
				// text
				textures[0].Write({text}, unselectedOnRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
				// up-left corner
				textures[1].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedOnRGBA), KTech::Point(0, 0));
				// up-right corner
				textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedOnRGBA), KTech::Point(1 + text.length(), 0));
				// bottom-left corner
				textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedOnRGBA), KTech::Point(0, 2));
				// bottom-right corner
				textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedOnRGBA), KTech::Point(1 + text.length(), 2));
				// up frame
				textures[5].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', unselectedOnRGBA), KTech::Point(1, 0));
				// left frame
				textures[6].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedOnRGBA), KTech::Point(0, 1));
				// bottom frame
				textures[7].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', unselectedOnRGBA), KTech::Point(1, 2));
				// right frame
				textures[8].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedOnRGBA), KTech::Point(1 + text.length(), 1));
			}
			else
			{
				textures.resize(9);
				// text
				textures[0].Write({text}, unselectedOffRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
				// up-left corner
				textures[1].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedOffRGBA), KTech::Point(0, 0));
				// up-right corner
				textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedOffRGBA), KTech::Point(1 + text.length(), 0));
				// bottom-left corner
				textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedOffRGBA), KTech::Point(0, 2));
				// bottom-right corner
				textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedOffRGBA), KTech::Point(1 + text.length(), 2));
				// up frame
				textures[5].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', unselectedOffRGBA), KTech::Point(1, 0));
				// left frame
				textures[6].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedOffRGBA), KTech::Point(0, 1));
				// bottom frame
				textures[7].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', unselectedOffRGBA), KTech::Point(1, 2));
				// right frame
				textures[8].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedOffRGBA), KTech::Point(1 + text.length(), 1));
			}
		}
		else
		{
			if (on)
			{
				textures.resize(1);
				// text
				textures[0].Write({text}, unselectedOnRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
			}
			else
			{
				textures.resize(1);
				// text
				textures[0].Write({text}, unselectedOffRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
			}
		}

		// Input handlers
		callbacksGroup->AddCallback(engine.io.RegisterCallback(key, std::bind(&Switch::InsideOnPress, this), false));
	}
};
