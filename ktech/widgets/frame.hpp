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

class Frame : public KTech::Widget
{
public:
	KTech::RGBA m_unselectedRGBA, m_selectedRGBA;

	Frame(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		KTech::Point pos,
		KTech::UPoint size,
		KTech::RGBA unselectedRGBA = KTech::RGBA( 150, 150, 150, 255 ),
		KTech::RGBA selectedRGBA = KTech::RGBA( 255, 255, 255, 255 ))
		: Widget(engine, ui, pos), m_unselectedRGBA(unselectedRGBA), m_selectedRGBA(selectedRGBA)
	{
		// Texture
		SetSize(size);
	}

	void SetSize(KTech::UPoint size)
	{
		KTech::RGBA tempRGBA = m_selected ? m_selectedRGBA : m_unselectedRGBA;
		m_textures.resize(8);
		// up-left corner
		m_textures[0].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 0));
		// up-right corner
		m_textures[1].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(size.x - 1, 0));
		// bottom-left corner
		m_textures[2].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, size.y - 1));
		// bottom-right corner
		m_textures[3].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(size.x - 1, size.y - 1));
		// up frame
		m_textures[4].Simple(KTech::UPoint(size.x - 2, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 0));
		// left frame
		m_textures[5].Simple(KTech::UPoint(1, size.y - 2), KTech::CellA('|', tempRGBA), KTech::Point(0, 1));
		// bottom frame
		m_textures[6].Simple(KTech::UPoint(size.x - 2, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, size.y - 1));
		// right frame
		m_textures[7].Simple(KTech::UPoint(1, size.y - 2), KTech::CellA('|', tempRGBA), KTech::Point(size.x - 1, 1));
	}

protected:
	virtual void OnSelect() override
	{
		RenderSelected();
	}

	virtual void OnDeselect() override
	{
		RenderUnselected();
	}

	void RenderSelected()
	{
		for (size_t i = 0; i < m_textures.size(); i++)
			m_textures[i].SetForeground(m_selectedRGBA);
	}  

	void RenderUnselected()
	{
		for (size_t i = 0; i < m_textures.size(); i++)
			m_textures[i].SetForeground(m_unselectedRGBA);
	}
};