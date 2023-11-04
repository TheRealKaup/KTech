#include "../../engine/engine.hpp"
#include "../../widgets/button.hpp"
#include "../../widgets/intfield.hpp"
#include "../../widgets/stringfield.hpp"
#include "../../widgets/switch.hpp"

using namespace Engine;

void Quit()
{
	Engine::running = false;
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
			countdownInvocation = Time::Invoke(std::bind(&UI::Countdown, this), 1, Time::Measurement::seconds);
		}
		else
			Quit();
	}

	void CancelCountdown(Layer* layer)
	{
		delete widgets[0];
		widgets[0] = new Button(layer, std::bind(&UI::StartExitCountdown, this, layer), Input::K::return_, Point(0, 0), "Exit", true);
		widgets[0]->Select();
		Time::CancelInvocation(countdownInvocation);
	}

	void StartExitCountdown(Layer* layer)
	{
		delete widgets[0];
		widgets[0] = new Button(layer, std::bind(&UI::CancelCountdown, this, layer), Input::K::return_, Point(0, 0), "Exiting in 3", true);
		countdown = 3;
		// Invoke countdown
		countdownInvocation = Time::Invoke(std::bind(&UI::Countdown, this), 1, Time::Measurement::seconds);
		// As can be seen, the first `return_` press does not also call `Exit()`! Nice!
		widgets[0]->Select();
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
		widgets[0] = new Button(layer, std::bind(&UI::StartExitCountdown, this, layer), Input::K::return_, Point(0, 0), "Exit", true);
		widgets[1] = new IntField(layer, nullptr, 0, 1000, "123", Point(0, 4), "Int = ", true);
		for (size_t i = 0; i < 1000; i++)
		{
			widgets[2] = new StringField(layer, nullptr, {keyrange_all}, Point(0, 8), "String = ", 4, "Test", true);
		}
		widgets[3] = new Switch(layer, nullptr, Input::K::return_, Point(0, 12), "Switch", false, true);

		widgets[currentWidget]->Select();

		Input::RegisterCallback(Input::K::Shift::tab, std::bind(&UI::MoveUp, this));
		Input::RegisterCallback(Input::K::tab, std::bind(&UI::MoveDown, this));
	}
};

int main()
{
	PrepareTerminal(UPoint(20, 20));
	Time::tpsLimit = 24;
	
	Map map;

	Layer layer;
	map.AddLayer(&layer);

	Camera camera({ 0, 0 }, { 20, 20 });
	map.AddCamera(&camera, true);

	std::thread t_inputLoop(Engine::Input::Loop);

	UI ui(&layer);

	while (Engine::running)
	{
		Time::StartThisTick();

		Input::Call();
		Time::CallInvocations();
		map.CallOnTicks();

		map.RenderReversed();
		map.Draw();
		Print();

		Engine::Time::WaitUntilNextTick();
	}
	
	Engine::ResetTerminal();
	exit(0);
}