/*
	widgetstest, a user interface example based on KTech.
	Copyright (C) 2023-2024 Ethan Kaufman (AKA Kaup)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../../ktech/ktech.hpp"
#include "../../ktech/widgets/button.hpp"
#include "../../ktech/widgets/intfield.hpp"
#include "../../ktech/widgets/stringfield.hpp"
#include "../../ktech/widgets/switch.hpp"

using namespace KTech;

Engine engine(UPoint(20, 20), 24);

void Quit()
{
	engine.running = false;
}

struct UITest
{
	Widget* widgets[4];
	
	ID<UI> ui;

	enum WidgetIndex
	{
		w_button,
		w_intfield,
		w_stringfield,
		w_switch,
	};
	size_t currentWidget = w_intfield;
	
	Time::Invocation* countdownInvocation;

	uint8_t countdown;
	// Countdown iteration
	void Countdown()
	{
		if (countdown > 0)
		{
			countdown--;
			widgets[0]->m_textures[0].Write({"Exiting in " + std::to_string(countdown)}, widgets[0]->m_textures[0].m_t[0][0].f, RGBAColors::transparent, Point(1, 1));
			countdownInvocation = engine.time.Invoke(std::bind(&UITest::Countdown, this), 1, Time::Measurement::seconds);
		}
		else
			Quit();
	}

	void CancelCountdown()
	{
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Delete", RGBColors::orange);
		delete widgets[0];
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Create", RGBColors::orange);
		widgets[0] = new Button(engine, ui, std::bind(&UITest::StartExitCountdown, this), Keys::return_, Point(0, 0), "Exit", true);
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Select", RGBColors::orange);
		widgets[0]->Select();
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Cancel invoke", RGBColors::orange);
		engine.time.CancelInvocation(countdownInvocation);
	}

	void StartExitCountdown()
	{
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Delete", RGBColors::orange);
		delete widgets[0];
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Create", RGBColors::orange);
		widgets[0] = new Button(engine, ui, std::bind(&UITest::CancelCountdown, this), Keys::return_, Point(0, 0), "Exiting in 3", true);
		countdown = 3;
		// Invoke countdown
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Invoke", RGBColors::orange);
		countdownInvocation = engine.time.Invoke(std::bind(&UITest::Countdown, this), 1, Time::Measurement::seconds);
		// As can be seen, the first `return_` press does not also call `Exit()`! Nice!
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Select", RGBColors::orange);
		widgets[0]->Select();
		engine.output.Log("(GAME) <UI::StartExitCountdown()> EOF", RGBColors::orange);
	}
	
	void MoveUp()
	{
		engine.output.Log("(GAME) <UI::MoveUp()> SOF", RGBColors::orange);
		widgets[currentWidget]->Deselect();
		if (currentWidget == w_button)
			currentWidget = w_switch;
		else
			currentWidget--;
		widgets[currentWidget]->Select();
		engine.output.Log("(GAME) <UI::MoveUp()> EOF", RGBColors::orange);
	}

	void MoveDown()
	{
		engine.output.Log("(GAME) <UI::MoveDown()> SOF", RGBColors::orange);
		widgets[currentWidget]->Deselect();
		if (currentWidget == w_switch)
			currentWidget = w_button;
		else
			currentWidget++;
		widgets[currentWidget]->Select();
		engine.output.Log("(GAME) <UI::MoveDown()> EOF", RGBColors::orange);
	}

	UITest(ID<UI> ui) : ui(ui)
	{
		widgets[0] = new Button(engine, ui, std::bind(&UITest::StartExitCountdown, this), Keys::return_, Point(0, 0), "Exit", true);
		widgets[1] = new IntField(engine, ui, nullptr, 0, 999, "123", Point(0, 4), "Int = ", true);
		widgets[2] = new StringField(engine, ui, nullptr, {keyrange_all}, Point(0, 8), "String = ", 4, "Test", true);
		widgets[3] = new Switch(engine, ui, nullptr, Keys::return_, Point(0, 12), "Switch", false, true);

		widgets[currentWidget]->Select();

		engine.input.RegisterCallback(Keys::Shift::tab, std::bind(&UITest::MoveUp, this));
		engine.input.RegisterCallback(Keys::tab, std::bind(&UITest::MoveDown, this));
	}
};

int main()
{
	// engine.output.PrintStartupNotice("widgetstest, a user interface example based on KTech.", "2024", "Ethan Kaufman", "widgetstest");

	Map map(engine);

	UI ui(engine, UPoint(20, 20));

	UITest uiTest(ui.m_id);

	while (engine.running)
	{
		// Calls
		engine.output.Log("(GAME) <main()::GameLoop> Call inputs", RGBColors::green);
		engine.input.CallHandlers();
		engine.output.Log("(GAME) <main()::GameLoop> Call invocations", RGBColors::green);
		engine.time.CallInvocations();
		engine.output.Log("(GAME) <main()::GameLoop> Call OnTicks", RGBColors::green);
		engine.memory.CallOnTicks();

		// Render, draw and print
		engine.output.Log("(GAME) <main()::GameLoop> Render", RGBColors::green);
		ui.Render();
		engine.output.Log("(GAME) <main()::GameLoop> Draw", RGBColors::green);
		engine.output.Draw(ui.m_image);
		engine.output.Log("(GAME) <main()::GameLoop> Print", RGBColors::green);
		engine.output.Print();

		engine.output.Log("(GAME) <main()::GameLoop> WaitUntilNextTicks", RGBColors::green);
		engine.time.WaitUntilNextTick();
	}
}