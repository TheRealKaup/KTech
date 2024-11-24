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
		KTech::RGBA unselectedOffRGBA = KTech::RGBAColors::gray,
		KTech::RGBA selectedOffRGBA = KTech::RGBAColors::white,
		KTech::RGBA unselectedOnRGBA = KTech::RGBAColors::Widgets::switchUnselectedOnGreen,
		KTech::RGBA selectedOnRGBA = KTech::RGBAColors::Widgets::switchSelectedOnGreen,
		KTech::RGBA downRGBA = KTech::RGBAColors::Widgets::buttonDownBlue)
		: Widget(engine, ui, pos), m_OnPress(std::move(OnPress)), m_on(on), m_unselectedOffRGBA(unselectedOffRGBA), m_selectedOffRGBA(selectedOffRGBA), m_unselectedOnRGBA(unselectedOnRGBA), m_selectedOnRGBA(selectedOnRGBA), m_downRGBA(downRGBA)
	{
		// Textures
		SetText(text, withFrame);
		// Input handlers
		m_callbacksGroup->AddCallback(engine.input.RegisterCallback(key, std::bind(&Switch::OnPress, this)));
	}

	virtual ~Switch()
	{
		if (m_downInvocation == nullptr)
		{
			engine.time.CancelInvocation(m_downInvocation);
		}
	}

	void SetText(const std::string& text, bool withFrame)
	{
		KTech::RGBA tempRGBA;
		if (m_on)
		{
			if (m_selected)
			{
				tempRGBA = m_selectedOnRGBA;
			}
			else
			{
				tempRGBA = m_unselectedOnRGBA;
			}
		}
		else
		{
			if (m_selected)
			{
				tempRGBA = m_selectedOffRGBA;
			}
			else
			{
				tempRGBA = m_unselectedOffRGBA;
			}
		}
		// Texture
		if (withFrame)
		{
			m_textures.resize(TEXTURES_SIZE_FRAMED);
			m_textures[ti_topLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 0));
			m_textures[ti_topRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length(), 0));
			m_textures[ti_bottomLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 2));
			m_textures[ti_bottomRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(1 + text.length(), 2));
			m_textures[ti_topFrame].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 0));
			m_textures[ti_leftFrame].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(0, 1));
			m_textures[ti_bottomFrame].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 2));
			m_textures[ti_rightFrame].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempRGBA), KTech::Point(1 + text.length(), 1));
		}
		else
		{
			m_textures.resize(TEXTURES_SIZE_FRAMELESS);
		}
		m_textures[ti_text].Write({text}, tempRGBA, KTech::RGBA(0, 0, 0, 0), KTech::Point(1, 1));
	}

	void SetValue(bool value)
	{
		m_on = value;
		RemovePressColor();
	}

protected:
	enum TextureIndex : size_t
	{
		ti_text,
		TEXTURES_SIZE_FRAMELESS,
		ti_topLeftCorner = TEXTURES_SIZE_FRAMELESS,
		ti_topRightCorner,
		ti_bottomLeftCorner,
		ti_bottomRightCorner,
		ti_topFrame,
		ti_leftFrame,
		ti_bottomFrame,
		ti_rightFrame,
		TEXTURES_SIZE_FRAMED
	};

	static constexpr size_t pressLength = 100;
	KTech::Time::Invocation* m_downInvocation = nullptr;

	void OnSelect() override
	{
		RenderSelected();
	}

	void OnDeselect() override
	{
		RenderUnselected();
	}

	auto OnPress() -> bool
	{
		m_on = !m_on;

		for (KTech::Texture& texture : m_textures)
		{
			texture.SetForeground(m_downRGBA);
		}

		engine.time.Invoke([this]() -> bool { return RemovePressColor(); }, pressLength, KTech::Time::Measurement::milliseconds);

		if (m_OnPress)
		{
			m_OnPress();
		}

		return true;
	}

	void RenderSelected()
	{
		if (m_on)
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.SetForeground(m_selectedOnRGBA);
			}
		}
		else
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.SetForeground(m_selectedOffRGBA);
			}
		}
	}

	void RenderUnselected()
	{
		if (m_on)
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.SetForeground(m_unselectedOnRGBA);
			}
		}
		else
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.SetForeground(m_unselectedOffRGBA);
			}
		}
	}

	auto RemovePressColor() -> bool
	{
		if (m_selected)
		{
			if (m_on)
			{
				for (KTech::Texture& texture : m_textures)
				{
					texture.SetForeground(m_selectedOnRGBA);
				}
			}
			else
			{
				for (KTech::Texture& texture : m_textures)
				{
					texture.SetForeground(m_selectedOffRGBA);
				}
			}
		}
		else
		{
			if (m_on)
			{
				for (KTech::Texture& texture : m_textures)
				{
					texture.SetForeground(m_unselectedOnRGBA);
				}
			}
			else
			{
				for (KTech::Texture& texture : m_textures)
				{
					texture.SetForeground(m_unselectedOffRGBA);
				}
			}
		}
		m_downInvocation = nullptr;
		return true;
	}
};
