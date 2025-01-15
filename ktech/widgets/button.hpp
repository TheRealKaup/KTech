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
	@brief Widget that calls a callback function when pressed.
*/
class Button : public KTech::Widget
{
public:
	std::function<void()> m_OnPress; //!< Function to call when pressed.

	/*!
		@brief Construct a `Button`.

		@param [in] engine Parent `Engine`.
		@param [in] ui `KTech::UI` to immediately enter.
		@param [in] OnPress Callback function.
		@param [in] key Key needed to press the `Button` (when it is selected).
		@param [in] position World position.
		@param [in] text Text within the `Button`.
		@param [in] withFrame Whether to add a frame around the text.
		@param [in] unselected Foreground (text and frame) color set when `Button` is unselected.
		@param [in] selected Foreground color set when `Button` is selected.
		@param [in] down Foreground color set when `Button` is being pressed.
	*/
	Button(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		std::function<void()> OnPress,
		const std::string& key = KTech::Keys::return_,
		KTech::Point position = KTech::Point(0, 0),
		const std::string& text = "Button",
		bool withFrame = false,
		KTech::RGBA unselected = KTech::RGBAColors::gray,
		KTech::RGBA selected = KTech::RGBAColors::white,
		KTech::RGBA down = KTech::RGBAColors::Widgets::buttonDownBlue)
		: Widget(engine, ui, position),
		m_OnPress(std::move(OnPress)),
		m_unselectedRGBA(unselected),
		m_selectedRGBA(selected),
		m_downRGBA(down),
		m_downInvocation(engine, [this]() -> bool { return RemovePressColor(); })
	{
		// Texture
		SetText(text, withFrame);
		// Input handlers
		m_callbacksGroup.RegisterCallback(key, [this]() -> bool { return InsideOnPress(); });
	}

	/*!
		@brief Change the displayed text.

		@param [in] text Text within the `Button`.
		@param [in] withFrame Whether to add a frame around the text.
	*/
	void SetText(const std::string& text, bool withFrame)
	{
		KTech::RGBA tempColor;
		if (m_downInvocation.m_active) // The button is down as it is invoked to remove down color
		{
			tempColor = m_downRGBA;
		}
		else if (m_selected)
		{
			tempColor = m_selectedRGBA;
		}
		else
		{
			tempColor = m_unselectedRGBA;
		}

		if (withFrame)
		{
			m_textures.resize(TEXTURES_SIZE_FRAMED);
			m_textures[ti_topLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempColor), KTech::Point(0, 0));
			m_textures[ti_topRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempColor), KTech::Point(1 + text.length(), 0));
			m_textures[ti_bottomLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempColor), KTech::Point(0, 2));
			m_textures[ti_bottomRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempColor), KTech::Point(1 + text.length(), 2));
			m_textures[ti_topFrame].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', tempColor), KTech::Point(1, 0));
			m_textures[ti_leftFrame].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempColor), KTech::Point(0, 1));
			m_textures[ti_bottomFrame].Simple(KTech::UPoint(text.length(), 1), KTech::CellA('-', tempColor), KTech::Point(1, 2));
			m_textures[ti_rightFrame].Simple(KTech::UPoint(1, 1), KTech::CellA('|', tempColor), KTech::Point(1 + text.length(), 1));
		}
		else
		{
			m_textures.resize(TEXTURES_SIZE_FRAMELESS);
		}
		// text
		m_textures[ti_text].Write({text}, tempColor, KTech::RGBAColors::transparent, KTech::Point(1, 1));
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

	KTech::RGBA m_unselectedRGBA, m_selectedRGBA, m_downRGBA;
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

	void RenderSelected()
	{
		for (KTech::Texture& texture : m_textures)
		{
			texture.SetForeground(m_selectedRGBA);
		}
	}

	void RenderUnselected()
	{
		for (KTech::Texture& texture : m_textures)
		{
			texture.SetForeground(m_unselectedRGBA);
		}
	}

	auto RemovePressColor() -> bool
	{
		if (m_selected)
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.SetForeground(m_selectedRGBA);
			}
		}
		else
		{
			for (KTech::Texture& texture : m_textures)
			{
				texture.SetForeground(m_unselectedRGBA);
			}
		}
		return true;
	}

	auto InsideOnPress() -> bool
	{
		for (KTech::Texture& texture : m_textures)
		{
			texture.SetForeground(m_downRGBA);
		}
		m_downInvocation.Invoke(pressLength, KTech::Time::Measurement::milliseconds);
		if (m_OnPress)
		{
			m_OnPress();
		}
		return true;
	}
};
