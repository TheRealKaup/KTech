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

/*!
	@brief Widget that displays framed text.
*/
class AboutBox : public KTech::Widget
{
public:
	/*!
		@brief Construct an `AboutBox`.

		@param engine Parent `Engine`.
		@param ui `UI` to immediately enter.
		@param position World position.
		@param text Text to display.
		@param foreground Foreground (text and frame) color.
		@param background Background color.
	*/
	AboutBox(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		KTech::Point position,
		const std::vector<std::string>& text,
		KTech::RGBA foreground = KTech::RGBAColors::black,
		KTech::RGBA background = KTech::RGBAColors::gray)
		: Widget(engine, ui, position), m_foregroundRGBA(foreground), m_backgroundRGBA(background)
	{
		// Texture
		SetText(text);
	}

	/*!
		@brief Change the displayed text.

		@param text Text to display.
	*/
	void SetText(const std::vector<std::string>& text)
	{
		m_textures.resize(TEXTURES_SIZE);
		m_textures[ti_text].Write(text, m_foregroundRGBA, m_backgroundRGBA, KTech::Point(1, 1))
			.ReplaceCharacter(' ', '\0')
			.SetBackground(m_backgroundRGBA);
		m_textures[ti_topLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(0, 0));
		m_textures[ti_topRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(m_textures[0].m_size.x + 1, 0));
		m_textures[ti_bottomLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(0, m_textures[0].m_size.y + 1));
		m_textures[ti_bottomRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(m_textures[0].m_size.x + 1, m_textures[0].m_size.y + 1));
		m_textures[ti_topFrame].Simple(KTech::UPoint(m_textures[0].m_size.x, 1), KTech::CellA('-', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(1, 0));
		m_textures[ti_leftFrame].Simple(KTech::UPoint(1, m_textures[0].m_size.y), KTech::CellA('|', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(0, 1));
		m_textures[ti_bottomFrame].Simple(KTech::UPoint(m_textures[0].m_size.x, 1), KTech::CellA('-', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(1, m_textures[0].m_size.y + 1));
		m_textures[ti_rightFrame].Simple(KTech::UPoint(1, m_textures[0].m_size.y), KTech::CellA('|', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(m_textures[0].m_size.x + 1, 1));
	}

private:
	enum TextureIndex : size_t
	{
		ti_text,
		ti_topLeftCorner,
		ti_topRightCorner,
		ti_bottomLeftCorner,
		ti_bottomRightCorner,
		ti_topFrame,
		ti_leftFrame,
		ti_bottomFrame,
		ti_rightFrame,
		TEXTURES_SIZE
	};

	KTech::RGBA m_foregroundRGBA, m_backgroundRGBA;
};