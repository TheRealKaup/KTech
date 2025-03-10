/*
	simpleplatform, a platform game example based on KTech.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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

using namespace KTech;

static constexpr UPoint viewport = UPoint(50, 30);

Engine engine(viewport, 24);

struct Character : Object
{
	KTech::ID<Layer> voidLayer;

	static constexpr int jumpStrength = 5;

	Camera cam;
	bool onGround = false;
	int yVelocity = 0;

	ID<Object> box;
	Animation jumpAnimation;

	Input::CallbacksGroup callbacksGroup;

	bool Jump()
	{
		if (onGround) {
			yVelocity = jumpStrength;
			onGround = false;
			jumpAnimation.Stop();
			jumpAnimation.Play();
			return true;
		}
		return false;
	}

	virtual bool OnTick() override
	{
		bool changed = false;

		bool left = false;
		bool right = false;

		bool priorOnGround = onGround;

		if (yVelocity > 0)
		{
			for (size_t i = 0; i < yVelocity; i++)
				changed = Move(Point(0, -1));
			yVelocity--;
		}
		else
		{
			onGround = !Move(Point(0, 1));
			if (!onGround)
				changed = true;
		}

		if (cam.m_pos != m_pos - Point(6, 6))
		{
			cam.m_pos = m_pos - Point(6, 6);
			changed = true;
		}

		return changed;
	}

	virtual void OnOverlap(Point p_dir, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider) override
	{
		if (engine.memory.objects[p_otherObject]->m_colliders[p_otherCollider].m_type == 2)
			box = p_otherObject;
	}

	virtual void OnOverlapExit(Point p_dir, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider) override
	{
		if (p_otherObject == box)
			box = nullID<Object>;
	}
	virtual void OnOverlappedExit(Point p_dir, size_t p_collider, ID<Object> p_otherObject, size_t p_otherCollider) override
	{
		if (p_otherObject == box)
			box = nullID<Object>;
	}

	bool PushBoxToDifferentLayer()
	{
		engine.output.Log("<Character::PushBoxToDifferentLayer()> Start of function...", RGBColors::red);
		if (engine.memory.objects.Exists(box))
		{
			engine.output.Log("<Character::PushBoxToDifferentLayer()> Moving object to voidLayer", RGBColors::red);
			engine.memory.objects[box]->EnterLayer(voidLayer);
			box = ID<Object>();
			return true;
		}
		return false;
	}

	Character(Engine& engine, const ID<Layer>& layer, const ID<Layer>& voidLayer)
		: Object(engine, Point(5, 2), "character"), voidLayer(voidLayer), cam(engine, KTech::Point( 0, 0 ), KTech::UPoint( 15, 15 )),
			jumpAnimation(engine, m_id, {
				Animation::Instruction(Animation::Instruction::Type::TextureSetPosition, 1, Point(0, 3)),
				Animation::Instruction(Animation::Instruction::Type::TextureShow, 1),
				Animation::Instruction(Animation::Instruction::Type::Delay, 2, Time::Measurement::ticks),
				Animation::Instruction(Animation::Instruction::Type::TextureMove, 1, Point(0, 1)),
				Animation::Instruction(Animation::Instruction::Type::Delay, 2, Time::Measurement::ticks),
				Animation::Instruction(Animation::Instruction::Type::TextureMove, 1, Point(0, 1)),
				Animation::Instruction(Animation::Instruction::Type::Delay, 2, Time::Measurement::ticks),
				Animation::Instruction(Animation::Instruction::Type::TextureHide, 1),
			}), callbacksGroup(engine, true)
	{
		KTech::Output::Log("<Character::Character()> Start of function...", RGBColors::red);
		m_textures.resize(2);
		m_textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, RGBA( 255, 255, 0, 255 ), RGBAColors::transparent, Point(0, 0)
		);
		m_textures[1].Simple(UPoint(3, 1), CellA('~', RGBAColors::gray, RGBA(255, 255, 255, 63)));
		m_textures[1].m_active = false;

		m_colliders.resize(2);
		m_colliders[0].ByTextureCharacter(m_textures[0], 1);
		m_colliders[1].Simple(KTech::UPoint(5, 5), 3, KTech::Point(-1, -1));

		KTech::Output::Log("<Character::Character()> Registering callbacks", RGBColors::red);
		callbacksGroup.RegisterCallback("w", std::bind(&Character::Jump, this));
		callbacksGroup.RegisterCallback("W", std::bind(&Character::Jump, this));
		callbacksGroup.RegisterCallback(" ", std::bind(&Character::Jump, this));
		callbacksGroup.RegisterCallback(KTech::Keys::up, std::bind(&Character::Jump, this));

		callbacksGroup.RegisterCallback("d", std::bind(&Character::Move, this, Point(1, 0)));
		callbacksGroup.RegisterCallback("D", std::bind(&Character::Move, this, Point(1, 0)));
		callbacksGroup.RegisterCallback(KTech::Keys::right, std::bind(&Character::Move, this, Point(1, 0)));

		callbacksGroup.RegisterCallback("a", std::bind(&Character::Move, this, Point(-1, 0)));
		callbacksGroup.RegisterCallback("A", std::bind(&Character::Move, this, Point(-1, 0)));
		callbacksGroup.RegisterCallback(KTech::Keys::left, std::bind(&Character::Move, this, Point(-1, 0)));

		callbacksGroup.RegisterCallback("f", std::bind(&Character::PushBoxToDifferentLayer, this));
		callbacksGroup.RegisterCallback("F", std::bind(&Character::PushBoxToDifferentLayer, this));

		KTech::Output::Log("<Character::Character()> Entering layer", RGBColors::red);
		EnterLayer(layer);

		cam.m_name = "character cam";

		KTech::Output::Log("<Character::Character()> End of function.", RGBColors::red);
	}
};

struct GravityBox : Object
{
	bool onGround = false;

	size_t fallingSpeed;

	virtual bool OnTick() override
	{
		// If caught, do not fall.
		bool moved = false;
		for (size_t i = 0; i < fallingSpeed; i++)
			if (Move(Point(0, 1)))
				moved = true;
		return moved;
	}

	GravityBox(Engine& engine, KTech::ID<Layer> layer, KTech::Point pos, size_t fallingSpeed = 1) : Object(engine, pos), fallingSpeed(fallingSpeed)
	{
		m_textures.resize(1);
		m_textures[0].Write(
			{
				"#-#",
				"|G|",
				"#-#"
			}, { 80, 55, 50, 255 }, { 190, 140, 115, 255 }, { 0, 0 }
		);

		m_colliders.resize(1);
		m_colliders[0].Simple(KTech::UPoint(3, 3), 2, KTech::Point(0, 0));

		EnterLayer(layer);
	}
};

struct AutoUpdatingText : Object
{
	float* m_data;
	static constexpr size_t historyLength = 10;
	float m_dataHistory[historyLength];
	size_t m_historyRotation = 0;

	virtual bool OnTick() override
	{
		m_dataHistory[m_historyRotation] = *m_data;
		float averageData = 0;
		size_t i;
		for (i = m_historyRotation; i < historyLength && m_dataHistory[i] != 0; i++)
			averageData += m_dataHistory[i];
		averageData /= (i - m_historyRotation);
		if (m_historyRotation >= historyLength - 1)
		{
			m_historyRotation = 0;
			m_textures[1].Write({std::to_string(averageData)}, RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 127), Point(m_textures[0].m_size.x, 0));
			return true;
		}
		else
		{
			m_historyRotation++;
			return false;
		}
	}

	AutoUpdatingText(Engine& engine, float* data, KTech::Point pos, const ID<Layer>& layer, std::string text)
		: Object(engine, pos), m_data(data)
	{
		EnterLayer(layer);
		m_textures.resize(2);
		m_textures[0].Write({ text },  RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 127), Point(0, 0));
	}
};

struct QuitUI : UI
{
	Button m_quitButton;

	Input::CallbacksGroup callbacksGroup;

	bool Toggle()
	{
		if (m_quitButton.m_selected)
		{
			m_quitButton.Hide();
			m_quitButton.Deselect();
			m_background.b = RGBA(0, 0, 0, 0);
			m_background.f = RGBA(0, 0, 0, 0);
		}
		else
		{
			m_quitButton.Show();
			m_quitButton.Select();
			m_background.b = RGBA(0, 0, 0, 100);
			m_background.f = RGBA(0, 0, 0, 100);
		}
		return true;
	}

	QuitUI(Engine& engine)
		: UI(engine, viewport), m_quitButton(engine, m_id, std::bind(&Engine::Quit, &engine), Keys::return_, Point(viewport.x / 2 - 3, viewport.y / 2 - 2), "Quit", true), callbacksGroup(engine, true)
	{
		AddWidget(m_quitButton.m_id);
		m_quitButton.Hide();
		m_quitButton.Deselect();
		callbacksGroup.RegisterCallback(Keys::escape, std::bind(&QuitUI::Toggle, this));
	}
};

bool charCamOn = false;

bool TurnOnCharacterCamera()
{
	charCamOn = !charCamOn;
	engine.output.Clear();
	return true;
}

int main()
{
	using namespace KTech;

	// It would be preferable to have the startup notice drawn within the engine's image, but this is temporary.
	// engine.output.PrintStartupNotice("simpleplatform, a platform game example based on KTech.", "2023", "Ethan Kaufman (AKA Kaup)", "simpleplatform");

	KTech::Output::Log("<main()> Creating collider types", RGBColors::blue);
	engine.collision.colliderTypes = {
		{ Collision::CR::B, Collision::CR::P, Collision::CR::P, Collision::CR::O }, // Heavy - 0
		{ Collision::CR::B, Collision::CR::P, Collision::CR::P, Collision::CR::O }, // Normal - 1
		{ Collision::CR::B, Collision::CR::B, Collision::CR::P, Collision::CR::O }, // Light - 2
		{ Collision::CR::O, Collision::CR::O, Collision::CR::O, Collision::CR::O }  // Overlappable - 3
	};

	KTech::Output::Log("<main()> Creating map", RGBColors::blue);
	KTech::Map map(engine);
	map.m_name = "map";

	KTech::Output::Log("<main()> Creating layer", RGBColors::blue);
	KTech::Layer layer(engine, map.m_id);
	layer.m_name = "layer";
	KTech::Output::Log("<main()> Creating voidLayer", RGBColors::blue);
	KTech::Layer voidLayer(engine, map.m_id);
	voidLayer.m_name = "voidLayer";

	KTech::Output::Log("<main()> Creating camera", RGBColors::blue);
	KTech::Camera camera(engine, Point(0, 0), viewport);
	camera.m_name = "camera";
	KTech::Output::Log("<main()> Adding camera to map", RGBColors::blue);
	// `Camera` to `ID<Camera>` cast overload
	map.AddCamera(camera.m_id);

	KTech::Object worldProps(engine, Point(1, 1), "worldProps");
	worldProps.m_name = "worldProps";
	worldProps.m_textures.resize(3);
	KTech::Output::Log("<main()> Loading examples/simpleplatform/assets/sky.ktecht", RGBColors::blue);
	worldProps.m_textures[0].File("examples/simpleplatform/assets/sky.ktecht", Point(0, 0));
	KTech::Output::Log("<main()> Loading examples/simpleplatform/assets/house.ktecht", RGBColors::blue);
	worldProps.m_textures[1].File("examples/simpleplatform/assets/house.ktecht", Point(18, 14));
	KTech::Output::Log("<main()> Loading examples/simpleplatform/assets/land.ktecht", RGBColors::blue);
	worldProps.m_textures[2].File("examples/simpleplatform/assets/land.ktecht", Point(0, 3));
	worldProps.m_colliders.resize(1);
	worldProps.m_colliders[0].ByTextureBackground(worldProps.m_textures[2], 0, 1);
	layer.AddObject(worldProps.m_id);

	KTech::Output::Log("<main()> Creating frame", RGBColors::blue);
	KTech::Object frame(engine, KTech::Point(0, 0), "frame");
	frame.m_name = "frame";
	frame.m_textures.resize(5);
	frame.m_textures[0].Simple(KTech::UPoint(1, viewport.y), KTech::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, 0));
	frame.m_textures[1].Simple(KTech::UPoint(1, viewport.y), KTech::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(viewport.x - 1, 0));
	frame.m_textures[2].Simple(KTech::UPoint(viewport.x, 1), KTech::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, 0));
	frame.m_textures[3].Simple(KTech::UPoint(viewport.x, 1), KTech::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, viewport.y - 1));
	frame.m_textures[4].Write(
		{
			"'WASD'/'Arrow keys' to move.",
			"'Space' to jump.",
			"'F' to throw a gravity box off the layer.",
			"'M' to turn on following camera.",
			"'ESC' to toggle menu."
		}, RGBAColors::black, RGBAColors::transparent, Point(2, 2))
		.Transform([](KTech::CellA& cell){ cell.b = RGBA(255, 255, 255, 100); });
	frame.m_colliders.resize(4);
	frame.m_colliders[0].Simple(KTech::UPoint(viewport.x, 1), 0, KTech::Point(0, 0));
	frame.m_colliders[1].Simple(KTech::UPoint(1, viewport.y), 0, KTech::Point(0, 0));
	frame.m_colliders[2].Simple(KTech::UPoint(viewport.x, 1), 0, KTech::Point(0, viewport.y - 1));
	frame.m_colliders[3].Simple(KTech::UPoint(1, viewport.y), 0, KTech::Point(viewport.x - 1, 0));
	layer.AddObject(frame.m_id);

	KTech::Output::Log("<main()> Creating character", RGBColors::blue);
	Character character(engine, layer.m_id, voidLayer.m_id);
	character.m_name = "character";
	map.AddCamera(character.cam.m_id);
std::function<void(int&)> func = [](int){};
	KTech::Output::Log("<main()> Creating GravityBoxes", RGBColors::blue);
	GravityBox gbA(engine, layer.m_id, Point(10, 5), 1);
	gbA.m_name = "gbA";
	GravityBox gbB(engine, layer.m_id, Point(15, 5), 2);
	gbB.m_name = "gbB";
	GravityBox gbC(engine, layer.m_id, Point(30, 5), 3);
	gbC.m_name = "gbC";

	KTech::Output::Log("<main()> Creating house", RGBColors::blue);
	KTech::Object house(engine, Point(20, 32));
	house.m_name = "house";
	layer.AddObject(house.m_id);

	house.m_textures.resize(1);
	house.m_textures[0].File("examples/simpleplatform/assets/house.ktecht", { 0, 0 });

	Input::CallbacksGroup callbacksGroup(engine, true);
	callbacksGroup.RegisterCallback("m", TurnOnCharacterCamera);
	callbacksGroup.RegisterCallback("M", TurnOnCharacterCamera);

	KTech::Output::Log("<main()> Creating darkLayer", RGBColors::blue);
	KTech::Layer darkLayer(engine, map.m_id);
	darkLayer.m_name = "darkLayer";
	darkLayer.m_alpha = 127;

	KTech::Output::Log("<main()> Creating AutoUpdatingText", RGBColors::blue);
	AutoUpdatingText tpsText(engine, &engine.time.tpsPotential, Point(2, 27), layer.m_id, "TPS: ");
	tpsText.m_name = "tpsText";

	QuitUI quitUI(engine);

	KTech::Output::Log("<main()> Entering game loop", RGBColors::blue);
	while (engine.running)
	{
		engine.input.CallCallbacks();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		if (engine.output.ShouldRenderThisTick())
		{
			if (charCamOn)
			{
				engine.output.Clear(); // Allows to make foreground darker
				camera.Render({ layer.m_id, voidLayer.m_id, darkLayer.m_id });
				engine.output.Draw(camera.m_image, camera.m_res, Point(0, 0), UPoint(0, 0), UPoint(0, 0), 100);
				character.cam.Render(map.m_layers);
				engine.output.Draw(
					character.cam.m_image,
					character.cam.m_res,
					Point(
						18 - (character.cam.m_pos.x <= 0 ? character.cam.m_pos.x - 1 : 0),
						9 - (character.cam.m_pos.y <= 0 ? character.cam.m_pos.y - 1 : 0)
					),
					UPoint(
						character.cam.m_pos.x <= 0 ? -character.cam.m_pos.x + 1 : 0,
						character.cam.m_pos.y <= 0 ? -character.cam.m_pos.y + 1 : 0
					),
					UPoint(
						character.cam.m_pos.x + character.cam.m_res.x >= viewport.x ? viewport.x - character.cam.m_pos.x - 1 : 0,
						character.cam.m_pos.y + character.cam.m_res.y >= viewport.y ? viewport.y - character.cam.m_pos.y - 1 : 0
					)
				);
			}
			else
			{
				camera.Render(map.m_layers);
				camera.Draw();
			}
			quitUI.Render();
			quitUI.Draw();
			engine.output.Print();
		}
		else if (engine.output.ShouldPrintThisTick())
		{
			engine.output.Print();
		}
		// KTech::Output::Log("<main()> WaitUntilNextTick()", RGBColors::blue);
		engine.time.WaitUntilNextTick();
	}
	KTech::Output::Log("<main()> End of function.", RGBColors::blue);

	engine.output.outputOnQuit.push_back("character.m_pos.y: " + std::to_string(character.m_pos.y) + "\n");
}