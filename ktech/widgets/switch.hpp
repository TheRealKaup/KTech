/*
	KTech, Kaup's C++ 2D terminal game engine library.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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

/*!
	@brief Widget that toggles a value and calls a callback function when pressed.
*/
class Switch : public KTech::Widget
{
public:
	bool m_on = false; //!< Toggled value.

	std::function<void()> m_OnPress; //!< Function to call when pressed.

	/*!
		@brief Construct a `Switch`.

		@param [in] engine Parent `Engine`.
		@param [in] ui `KTech::UI` to immediately enter.
		@param [in] OnPress Callback function.
		@param [in] key Key needed to press the `Switch` (when it is selected).
		@param [in] position World position.
		@param [in] text Text within the `Switch`.
		@param [in] on Whether the `Switch` is on or off by default.
		@param [in] withFrame Whether to add a frame around the text.
		@param [in] unselectedOff Foreground (text and frame) color set when `Switch` is unselected and off.
		@param [in] selectedOff Foreground color set when `Switch` is selected and off.
		@param [in] unselectedOn Foreground color set when `Switch` is unselected and on.
		@param [in] selectedOn Foreground color set when `Switch` is selected and on.
		@param [in] down Foreground color set when `Switch` is being pressed.
	*/
	Switch(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnPress,
		const std::string& key = KTech::Keys::return_,
		KTech::Point position = { 0, 0 },
		const std::string& text = "Switch",
		bool on = false,
		bool withFrame = false,
		KTech::RGBA unselectedOff = KTech::RGBAColors::gray,
		KTech::RGBA selectedOff = KTech::RGBAColors::white,
		KTech::RGBA unselectedOn = KTech::RGBAColors::Widgets::switchUnselectedOnGreen,
		KTech::RGBA selectedOn = KTech::RGBAColors::Widgets::switchSelectedOnGreen,
		KTech::RGBA down = KTech::RGBAColors::Widgets::buttonDownBlue)
		: Widget(engine, ui, position),
		m_OnPress(std::move(OnPress)),
		m_on(on),
		m_unselectedOffRGBA(unselectedOff),
		m_selectedOffRGBA(selectedOff),
		m_unselectedOnRGBA(unselectedOn),
		m_selectedOnRGBA(selectedOn),
		m_downRGBA(down),
		m_downInvocation(engine, [this]() -> bool { return RemovePressColor(); })
	{
		// Textures
		SetText(text, withFrame);
		// Input handlers
		m_callbackGroup.RegisterCallback(key, [this]() -> bool { return this->OnPress(); });
	}

	/*!
		@brief Change the displayed text.

		@param [in] text Text within the `Switch`.
		@param [in] withFrame Whether to add a frame around the text.
	*/
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

	/*!
		@brief Set the toggled value.
		@param [in] value On or off.
	*/
	void SetValue(bool value)
	{
		m_on = value;
		RemovePressColor();
	}

private:
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

	KTech::RGBA m_unselectedOffRGBA, m_selectedOffRGBA, m_unselectedOnRGBA, m_selectedOnRGBA, m_downRGBA;
	static constexpr size_t pressLength = 100;
	KTech::Time::Invocation m_downInvocation;

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
			texture.Transform([&](KTech::CellA& cell){ cell.f = m_downRGBA; });
		}

		m_downInvocation.Invoke(pressLength, KTech::Time::Measurement::milliseconds);

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
				texture.Transform([&](KTech::CellA& cell){ cell.f = m_selectedOnRGBA; });
			}
		}
		else
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.Transform([&](KTech::CellA& cell){ cell.f = m_selectedOffRGBA; });
			}
		}
	}

	void RenderUnselected()
	{
		if (m_on)
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.Transform([&](KTech::CellA& cell){ cell.f = m_unselectedOnRGBA; });
			}
		}
		else
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.Transform([&](KTech::CellA& cell){ cell.f = m_unselectedOffRGBA; });
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
					texture.Transform([&](KTech::CellA& cell){ cell.f = m_selectedOnRGBA; });
				}
			}
			else
			{
				for (KTech::Texture& texture : m_textures)
				{
					texture.Transform([&](KTech::CellA& cell){ cell.f = m_selectedOffRGBA; });
				}
			}
		}
		else
		{
			if (m_on)
			{
				for (KTech::Texture& texture : m_textures)
				{
					texture.Transform([&](KTech::CellA& cell){ cell.f = m_unselectedOnRGBA; });
				}
			}
			else
			{
				for (KTech::Texture& texture : m_textures)
				{
					texture.Transform([&](KTech::CellA& cell){ cell.f = m_unselectedOffRGBA; });
				}
			}
		}
		return true;
	}
};
