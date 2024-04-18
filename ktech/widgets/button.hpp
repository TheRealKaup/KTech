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

class Button : public KTech::Widget
{
public:
	std::function<void()> m_OnPress;
	
	KTech::RGBA m_unselectedRGBA, m_selectedRGBA, m_downRGBA;

private:
	void RemovePressColor()
	{
		if (m_selected)
			for (size_t i = 0; i < m_textures.size(); i++)
				m_textures[i].SetForeground(m_selectedRGBA);
		else
			for (size_t i = 0; i < m_textures.size(); i++)
				m_textures[i].SetForeground(m_unselectedRGBA);
	}

	void InsideOnPress()
	{
		for (size_t i = 0; i < m_textures.size(); i++)
			m_textures[i].SetForeground(m_downRGBA);
		
		engine.time.Invoke(std::bind(&Button::RemovePressColor, this), 100, KTech::Time::Measurement::milliseconds);

		if (m_OnPress)
			m_OnPress();
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

	Button(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnPress,
		std::string key = KTech::Keys::return_,
		KTech::Point pos = { 0, 0 },
		const std::string& text = "Button",
		bool withFrame = false,
		KTech::RGBA unselectedRGBA = KTech::RGBA( 150, 150, 150, 255 ),
		KTech::RGBA selectedRGBA = KTech::RGBA( 255, 255, 255, 255 ),
		KTech::RGBA downRGBA = KTech::RGBA(150, 150, 255, 255))
		: Widget(engine, ui, pos), m_OnPress(OnPress), m_unselectedRGBA(unselectedRGBA), m_selectedRGBA(selectedRGBA), m_downRGBA(downRGBA)
	{
		// Texture
		if (withFrame)
		{	
			m_textures.resize(9);
			// text
			m_textures[0].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
			// up-left corner
			m_textures[1].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 0));
			// up-right corner
			m_textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length(), 0));
			// bottom-left corner
			m_textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(0, 2));
			// bottom-right corner
			m_textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', unselectedRGBA), KTech::Point(1 + text.length(), 2));
			// up frame
			m_textures[5].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 0));
			// left frame
			m_textures[6].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(0, 1));
			// bottom frame
			m_textures[7].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', unselectedRGBA), KTech::Point(1, 2));
			// right frame
			m_textures[8].Simple(KTech::UPoint(1, 1), KTech::CellA('|', unselectedRGBA), KTech::Point(1 + text.length(), 1));
		}
		else
		{
			m_textures.resize(1);
			// text
			m_textures[0].Write({text}, unselectedRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
		}

		// Input handlers
		// This is insane.
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(key, std::bind(&Button::InsideOnPress, this), false));
	}
};
