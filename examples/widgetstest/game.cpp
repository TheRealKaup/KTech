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

/*
	WARNING:
	The way this program is written is not advised, as it misses some new, preferable, alternative features of KTech.
	A rewrite is warranted.
*/

#include "../../ktech/ktech.hpp"
#include "../../ktech/widgets/button.hpp"
#include "../../ktech/widgets/frame.hpp"
#include "../../ktech/widgets/intfield.hpp"
#include "../../ktech/widgets/stringfield.hpp"
#include "../../ktech/widgets/switch.hpp"
#include "../../ktech/widgets/aboutbox.hpp"

using namespace KTech;

Engine engine(UPoint(19, 19), 24);

void Quit()
{
	engine.running = false;
}

struct UITest
{
	enum WidgetIndex
	{
		w_frame,
		w_showNotice,
		w_button,
		w_intfield,
		w_stringfield,
		w_switch,
		w_notice,
		WIDGETS_SIZE
	};

	Widget* widgets[WIDGETS_SIZE];
	size_t currentWidget = w_intfield;

	ID<UI> ui;

	Time::Invocation* countdownInvocation;

	uint8_t countdown;
	// Countdown iteration
	bool Countdown()
	{
		if (countdown > 0)
		{
			countdown--;
			((Button*)widgets[w_button])->SetText("Exiting in " + std::to_string(countdown), true);
			countdownInvocation = engine.time.Invoke(std::bind(&UITest::Countdown, this), 1, Time::Measurement::seconds);
		}
		else
			Quit();
		return true;
	}

	void CancelCountdown()
	{
		((Button*)widgets[w_button])->SetText("Exit", true);
		((Button*)widgets[w_button])->m_OnPress = std::bind(&UITest::StartExitCountdown, this);
		engine.time.CancelInvocation(countdownInvocation);

		((IntField*)widgets[w_intfield])->SetValue("789");
		((StringField*)widgets[w_stringfield])->SetValue("abcd");
		((Switch*)widgets[w_switch])->SetValue(true);
	}

	void StartExitCountdown()
	{
		((Button*)widgets[w_button])->SetText("Exiting in 3", true);
		((Button*)widgets[w_button])->m_OnPress = std::bind(&UITest::CancelCountdown, this);
		countdown = 3;
		// Invoke countdown
		engine.output.Log("(GAME) <UI::StartExitCountdown()> Invoke", RGBColors::orange);
		countdownInvocation = engine.time.Invoke(std::bind(&UITest::Countdown, this), 1, Time::Measurement::seconds);
	}

	bool MoveUp()
	{
		engine.output.Log("(GAME) <UI::MoveUp()> SOF", RGBColors::orange);
		widgets[currentWidget]->Deselect();
		if (currentWidget == w_showNotice)
			currentWidget = w_switch;
		else
			currentWidget--;
		widgets[currentWidget]->Select();
		engine.output.Log("(GAME) <UI::MoveUp()> EOF", RGBColors::orange);
		return true;
	}

	bool MoveDown()
	{
		engine.output.Log("(GAME) <UI::MoveDown()> SOF", RGBColors::orange);
		widgets[currentWidget]->Deselect();
		if (currentWidget == w_switch)
			currentWidget = w_showNotice;
		else
			currentWidget++;
		widgets[currentWidget]->Select();
		engine.output.Log("(GAME) <UI::MoveDown()> EOF", RGBColors::orange);
		return true;
	}

	void SetNotice()
	{
		if (((Switch*)widgets[w_showNotice])->m_on)
			widgets[w_notice]->Show();
		else
			widgets[w_notice]->Hide();
	}

	UITest(ID<UI> ui) : ui(ui)
	{
		widgets[w_frame] = new Frame(engine, ui, Point(0, 0), UPoint(19, 19));
		widgets[w_showNotice] = new Switch(engine, ui, std::bind(&UITest::SetNotice, this), Keys::return_, Point(3, -1), "Show\1Notice", false, false);
		widgets[w_button] = new Button(engine, ui, std::bind(&UITest::StartExitCountdown, this), Keys::return_, Point(2, 2), "Exit", true);
		widgets[w_intfield] = new IntField(engine, ui, nullptr, 0, 999, "123", Point(2, 6), "Int = ", true);
		widgets[w_stringfield] = new StringField(engine, ui, nullptr, {keyrange_all}, Point(2, 10), "String = ", 4, "Test", true);
		widgets[w_switch] = new Switch(engine, ui, nullptr, Keys::return_, Point(2, 14), "Switch", false, true);
		widgets[w_notice] = new AboutBox(engine, ui, Point(1, 1),
			{
				"widgetstest, a",
				"user interface",
				"example based",
				"on KTech.",
				"Copyright (C)",
				"2023-2024 Kaup",
				"",
				"GPLv3-or-later.",
				"NO WARRANTY",
				"PROVIDED.",
				"See COPYING",
				"file for more",
				"information."
			}
		);
		SetNotice();
		widgets[currentWidget]->Select();

		engine.input.RegisterCallback(Keys::Shift::tab, std::bind(&UITest::MoveUp, this));
		engine.input.RegisterCallback(Keys::tab, std::bind(&UITest::MoveDown, this));
	}

	~UITest()
	{
		engine.output.outputOnQuit.push_back(
			"Results:\n" +
			std::to_string(((IntField*)widgets[w_intfield])->m_number) + '\n' +
			((StringField*)widgets[w_stringfield])->m_string + '\n' +
			(((Switch*)widgets[w_switch])->m_on ? "True\n" : "False\n")
		);
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
		engine.input.CallCallbacks();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		if (engine.output.ShouldRenderThisTick())
		{
			// Render, draw and print
			ui.Render();
			engine.output.Clear();
			ui.Render();
			engine.output.Print();
		}
		else if (engine.output.ShouldPrintThisTick())
		{
			engine.output.Print();
		}

		engine.time.WaitUntilNextTick();
	}
}