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

class Switch : public KTech::Widget
{
public:
	bool m_on = false;

	std::function<void()> m_OnPress;
	
	KTech::RGBA m_unselectedOffRGBA, m_selectedOffRGBA, m_unselectedOnRGBA, m_selectedOnRGBA, m_downRGBA;

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
		: Widget(engine, ui, pos), m_OnPress(OnPress), m_on(on), m_unselectedOffRGBA(unselectedOffRGBA), m_selectedOffRGBA(selectedOffRGBA), m_unselectedOnRGBA(unselectedOnRGBA), m_selectedOnRGBA(selectedOnRGBA), m_downRGBA(downRGBA)
	{
		// Textures
		SetText(text, withFrame);
		// Input handlers
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(key, std::bind(&Switch::OnPress, this), false));
	}

	virtual ~Switch()
	{
		if (m_downInvocation == nullptr)
			engine.time.CancelInvocation(m_downInvocation);
	}

	void SetText(const std::string& text, bool withFrame)
	{
		KTech::RGBA tempRGBA;
		if (m_on)
		{
			if (m_selected)
				tempRGBA = m_selectedOnRGBA;
			else
				tempRGBA = m_unselectedOnRGBA;
		}
		else
		{
			if (m_selected)
				tempRGBA = m_selectedOffRGBA;
			else
				tempRGBA = m_unselectedOffRGBA;	
		}
		// Texture
		if (withFrame)
		{
			m_textures.resize(9);
			// up-left corner
			m_textures[1].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 0));
			// up-right corner
			m_textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length(), 0));
			// bottom-left corner
			m_textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 2));
			// bottom-right corner
			m_textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length(), 2));
			// up frame
			m_textures[5].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 0));
			// left frame
			m_textures[6].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(0, 1));
			// bottom frame
			m_textures[7].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 2));
			// right frame
			m_textures[8].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(1 + text.length(), 1));
		}
		else
			m_textures.resize(1);
		// text
		m_textures[0].Write({text}, tempRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
	}

	void SetValue(bool value)
	{
		m_on = value;
		RemovePressColor();
	}

protected:
	KTech::Time::Invocation* m_downInvocation = nullptr;

	virtual void OnSelect() override
	{
		RenderSelected();
	}

	virtual void OnDeselect() override
	{
		RenderUnselected();
	}

	void OnPress()
	{
		m_on = !m_on;

		for (KTech::Texture& texture : m_textures)
			texture.SetForeground(m_downRGBA);
		
		engine.time.Invoke(std::bind(&Switch::RemovePressColor, this), 100, KTech::Time::Measurement::milliseconds);

		if (m_OnPress)
			m_OnPress();
	}

	void RenderSelected()
	{
		if (m_on)
			for (KTech::Texture& texture : m_textures)
				texture.SetForeground(m_selectedOnRGBA);
		else
			for (KTech::Texture& texture : m_textures)
				texture.SetForeground(m_selectedOffRGBA);
	}  

	void RenderUnselected()
	{
		if (m_on)
			for (KTech::Texture& texture : m_textures)
				texture.SetForeground(m_unselectedOnRGBA);
		else
			for (KTech::Texture& texture : m_textures)
				texture.SetForeground(m_unselectedOffRGBA);
	}

	void RemovePressColor()
	{
		if (m_selected)
		{
			if (m_on)
				for (KTech::Texture& texture : m_textures)
					texture.SetForeground(m_selectedOnRGBA);
			else
				for (KTech::Texture& texture : m_textures)
					texture.SetForeground(m_selectedOffRGBA);
		}
		else
		{
			if (m_on)
				for (KTech::Texture& texture : m_textures)
					texture.SetForeground(m_unselectedOnRGBA);
			else
				for (KTech::Texture& texture : m_textures)
					texture.SetForeground(m_unselectedOffRGBA);
		}
		m_downInvocation = nullptr;
	}
};
