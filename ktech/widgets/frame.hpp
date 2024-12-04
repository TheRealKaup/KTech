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
	@brief Widget that looks like a frame.
*/
class Frame : public KTech::Widget
{
public:
	/*!
		@brief Construct a `Frame`.

		@param [in] engine Parent `Engine`.
		@param [in] ui `KTech::UI` to immediately enter.
		@param [in] position World position.
		@param [in] size Frame's size.
		@param [in] unselected Foreground (text and frame) color set when `Frame` is unselected.
		@param [in] selected Foreground color set when `Frame` is selected.
	*/
	Frame(KTech::Engine& engine,
		KTech::ID<KTech::UI> ui,
		KTech::Point position,
		KTech::UPoint size,
		KTech::RGBA unselected = KTech::RGBAColors::gray,
		KTech::RGBA selected = KTech::RGBAColors::white)
		: Widget(engine, ui, position), m_unselectedRGBA(unselected), m_selectedRGBA(selected)
	{
		// Texture
		SetSize(size);
	}

	/*!
		@brief Resize the frame.

		@param [in] size New size.
	*/
	void SetSize(KTech::UPoint size)
	{
		KTech::RGBA tempRGBA = m_selected ? m_selectedRGBA : m_unselectedRGBA;
		m_textures.resize(TEXTURES_SIZE);
		m_textures[ti_topLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, 0));
		m_textures[ti_topRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(size.x - 1, 0));
		m_textures[ti_bottomLeftCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(0, size.y - 1));
		m_textures[ti_bottomRightCorner].Simple(KTech::UPoint(1, 1), KTech::CellA('#', tempRGBA), KTech::Point(size.x - 1, size.y - 1));
		m_textures[ti_topFrame].Simple(KTech::UPoint(size.x - 2, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, 0));
		m_textures[ti_leftFrame].Simple(KTech::UPoint(1, size.y - 2), KTech::CellA('|', tempRGBA), KTech::Point(0, 1));
		m_textures[ti_bottomFrame].Simple(KTech::UPoint(size.x - 2, 1), KTech::CellA('-', tempRGBA), KTech::Point(1, size.y - 1));
		m_textures[ti_rightFrame].Simple(KTech::UPoint(1, size.y - 2), KTech::CellA('|', tempRGBA), KTech::Point(size.x - 1, 1));
	}

private:
	enum TextureIndex : size_t
	{
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

	KTech::RGBA m_unselectedRGBA, m_selectedRGBA;

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
};