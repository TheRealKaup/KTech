/*
	widgetstest, a user interface example based on KTech.
	Copyright (C) 2023 E. Kaufman (AKA Kaup)

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

struct UI
{
	Widget* widgets[4];

	enum WidgetIndex
	{
		w_button,
		w_intfield,
		w_stringfield,
		w_switch,
	};
	size_t currentWidget = w_stringfield;
	
	Time::Invocation* countdownInvocation;

	uint8_t countdown;
	// Countdown iteration
	void Countdown()
	{
		if (countdown > 0)
		{
			countdown--;
			widgets[0]->obj.textures[0].Write({"Exiting in " + std::to_string(countdown)}, widgets[0]->obj.textures[0].t[0][0].f, RGBAColors::transparent, Point(1, 1));
			countdownInvocation = engine.time.Invoke(std::bind(&UI::Countdown, this), 1, Time::Measurement::seconds);
		}
		else
			Quit();
	}

	void CancelCountdown(Layer* layer)
	{
		delete widgets[0];
		widgets[0] = new Button(layer, std::bind(&UI::StartExitCountdown, this, layer), Keys::return_, Point(0, 0), "Exit", true);
		widgets[0]->Select();
		engine.time.CancelInvocation(countdownInvocation);
	}

	void StartExitCountdown(Layer* layer)
	{
		engine.io.Log("<UI::StartExitCountdown()> Delete", RGBColors::orange);
		delete widgets[0];
		engine.io.Log("<UI::StartExitCountdown()> Create", RGBColors::orange);
		widgets[0] = new Button(layer, std::bind(&UI::CancelCountdown, this, layer), Keys::return_, Point(0, 0), "Exiting in 3", true);
		countdown = 3;
		// Invoke countdown
		engine.io.Log("<UI::StartExitCountdown()> Invoke", RGBColors::orange);
		countdownInvocation = engine.time.Invoke(std::bind(&UI::Countdown, this), 1, Time::Measurement::seconds);
		// As can be seen, the first `return_` press does not also call `Exit()`! Nice!
		engine.io.Log("<UI::StartExitCountdown()> Select", RGBColors::orange);
		widgets[0]->Select();
		engine.io.Log("<UI::StartExitCountdown()> EOF", RGBColors::orange);
	}
	
	void MoveUp()
	{
		widgets[currentWidget]->Deselect();
		if (currentWidget == w_button)
			currentWidget = w_switch;
		else
			currentWidget--;
		widgets[currentWidget]->Select();
	}

	void MoveDown()
	{
		widgets[currentWidget]->Deselect();
		if (currentWidget == w_switch)
			currentWidget = w_button;
		else
			currentWidget++;
		widgets[currentWidget]->Select();
	}

	UI(Layer* layer)
	{
		widgets[0] = new Button(layer, std::bind(&UI::StartExitCountdown, this, layer), Keys::return_, Point(0, 0), "Exit", true);
		widgets[1] = new IntField(layer, nullptr, 0, 1000, "123", Point(0, 4), "Int = ", true);
		widgets[2] = new StringField(layer, nullptr, {keyrange_all}, Point(0, 8), "String = ", 4, "Test", true);
		widgets[3] = new Switch(layer, nullptr, Keys::return_, Point(0, 12), "Switch", false, true);

		widgets[currentWidget]->Select();

		engine.io.RegisterCallback(Keys::Shift::tab, std::bind(&UI::MoveUp, this));
		engine.io.RegisterCallback(Keys::tab, std::bind(&UI::MoveDown, this));
	}
};

int main()
{
	Map map(&engine);
	Layer layer(&map);

	Camera camera({ 0, 0 }, { 20, 20 });
	map.AddCamera(&camera, true);

	UI ui(&layer);

	while (engine.running)
	{
		engine.io.Log("<main()::GameLoop> StartThisTick", RGBColors::green);
		engine.time.StartThisTick();

		engine.io.Log("<main()::GameLoop> Call inputs", RGBColors::green);
		engine.io.Call();
		engine.io.Log("<main()::GameLoop> Call invocations", RGBColors::green);
		engine.time.CallInvocations();
		engine.io.Log("<main()::GameLoop> Call OnTicks", RGBColors::green);
		map.CallOnTicks();

		engine.io.Log("<main()::GameLoop> Render", RGBColors::green);
		map.Render();
		engine.io.Log("<main()::GameLoop> Draw", RGBColors::green);
		engine.io.Draw(camera.image);
		engine.io.Log("<main()::GameLoop> Print", RGBColors::green);
		engine.io.Print();

		engine.io.Log("<main()::GameLoop> WaitUntilNextTicks", RGBColors::green);
		engine.time.WaitUntilNextTick();
	}
}