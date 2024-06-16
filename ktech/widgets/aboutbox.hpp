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

class AboutBox : public KTech::Widget
{
public:
	KTech::RGBA m_foregroundRGBA, m_backgroundRGBA;

	AboutBox(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		KTech::Point pos,
		const std::vector<std::string>& text,
		KTech::RGBA foregroundRGBA = KTech::RGBA( 0, 0, 0, 255 ),
		KTech::RGBA backgroundRGBA = KTech::RGBAColors::gray)
		: Widget(engine, ui, pos), m_foregroundRGBA(foregroundRGBA), m_backgroundRGBA(backgroundRGBA)
	{
		// Texture
		SetText(text);
	}

	void SetText(const std::vector<std::string>& text)
	{
		m_textures.resize(9);
		// text
		m_textures[0].Write(text, m_foregroundRGBA, m_backgroundRGBA, KTech::Point(1, 1)).ReplaceCharacter(' ', '\0').SetBackground(m_backgroundRGBA);
		// up-left corner
		m_textures[1].Simple(KTech::UPoint(1, 1), KTech::CellA('#', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(0, 0));
		// up-right corner
		m_textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(m_textures[0].m_size.x + 1, 0));
		// bottom-left corner
		m_textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(0, m_textures[0].m_size.y + 1));
		// bottom-right corner
		m_textures[4].Simple(KTech::UPoint(1, 1), KTech::CellA('#', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(m_textures[0].m_size.x + 1, m_textures[0].m_size.y + 1));
		// up frame
		m_textures[5].Simple(KTech::UPoint(m_textures[0].m_size.x, 1), KTech::CellA('-', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(1, 0));
		// left frame
		m_textures[6].Simple(KTech::UPoint(1, m_textures[0].m_size.y), KTech::CellA('|', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(0, 1));
		// bottom frame
		m_textures[7].Simple(KTech::UPoint(m_textures[0].m_size.x, 1), KTech::CellA('-', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(1, m_textures[0].m_size.y + 1));
		// right frame
		m_textures[8].Simple(KTech::UPoint(1, m_textures[0].m_size.y), KTech::CellA('|', m_foregroundRGBA, m_backgroundRGBA), KTech::Point(m_textures[0].m_size.x + 1, 1));
	}
};