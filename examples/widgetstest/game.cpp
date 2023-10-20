#include "../../engine/engine.hpp"
#include "../../widgets/button.hpp"
#include "../../widgets/intfield.hpp"
#include "../../widgets/stringfield.hpp"
#include "../../widgets/switch.hpp"

using namespace Engine;

struct UI
{
	static void Exit()
	{
		Engine::running = false;
	}

	void ConfirmExit(Layer* layer)
	{
		delete widgets[0];
		widgets[0] = new Button(layer, Exit, Input::K::return_, Point(0, 0), "Sure?", true);
		// As can be seen, the first `return_` press does not also call `Exit()`! Nice!
		widgets[0]->Select();
	}
	
	Widget* widgets[4];

	enum WidgetIndex
	{
		w_button,
		w_intfield,
		w_stringfield,
		w_switch,
	};
	size_t currentWidget = w_stringfield;
	
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
		widgets[0] = new Button(layer, std::bind(&UI::ConfirmExit, this, layer), Input::K::return_, Point(0, 0), "Exit", true);
		widgets[1] = new IntField(layer, nullptr, 0, 1000, "123", Point(0, 4), "Int = ", true);
		widgets[2] = new StringField(layer, nullptr, {Characters_All}, Point(0, 8), "String = ", 4, "Test", true);
		widgets[3] = new Switch(layer, nullptr, Input::K::return_, Point(0, 12), "Switch", false, true);
	
		widgets[currentWidget]->Select();

		Input::RegisterCallback(Input::K::Shift::tab, std::bind(&UI::MoveUp, this));
		Input::RegisterCallback(Input::K::tab, std::bind(&UI::MoveDown, this));
	}
};

int main()
{
	PrepareTerminal(UPoint(20, 20));
	Time::tps = 24;
	
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

		map.Render();
		map.Draw();
		Print();

		Engine::Time::WaitUntilNextTick();
	}
	
	Engine::ResetTerminal();
	exit(0);
}