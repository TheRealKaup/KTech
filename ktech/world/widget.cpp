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

#include "widget.hpp"
#include "ui.hpp"
#include "../engine/engine.hpp"

KTech::Widget::Widget(KTech::Engine& engine, KTech::ID<KTech::UI> ui, KTech::Point pos)
	: engine(engine), pos(pos), callbacksGroup(engine.io.CreateCallbacksGroup(false))
{
	engine.memory.widgets.Add(this);
	EnterUI(ui);
}

KTech::Widget::~Widget()
{
	callbacksGroup->DeleteCallbacks();
	if (engine.memory.uis.Exists(parentUI))
		engine.memory.uis[parentUI]->RemoveWidget(id);
	engine.memory.widgets.Remove(id);
}

void KTech::Widget::EnterUI(ID<UI> ui)
{
	if (engine.memory.uis.Exists(parentUI))
		engine.memory.uis[parentUI]->RemoveWidget(id);
	engine.memory.uis[ui]->AddWidget(id);
}

void KTech::Widget::Select()
{
	selected = true;
	callbacksGroup->Enable();
	RenderSelected();
};

void KTech::Widget::Deselect()
{
	selected = false;
	callbacksGroup->Disable();
	RenderUnselected();
}