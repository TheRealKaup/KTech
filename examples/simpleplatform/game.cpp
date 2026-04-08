/*
	simpleplatform, a platform game example based on KTech.
	Copyright (C) 2023-2026 Ethan Kaufman (AKA Kaup)

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

// NOLINTBEGIN(readability-*, bugprone-narrowing-conversions, bugprone-easily-swappable-parameters)

#include "../../ktech/ktech.hpp"
#include "../../ktech/widgets/button.hpp"

using namespace KTech;

namespace
{
	static UPoint viewport = UPoint(50, 30);

	class Character : public Object
	{
	public:
		Camera m_cam;

		Character(Engine& engine, const ID<Layer>& layer, const ID<Layer>& voidLayer)
			: Object(engine, {.x = 5, .y = 2}, "character"),
			  m_voidLayer(voidLayer),
			  m_cam(engine, Point{.x = 0, .y = 0}, UPoint{.x = 15, .y = 15}),
			  m_jumpAnimation(
				  engine,
				  m_id,
				  {
					  Animation::Instruction(Animation::Instruction::Type::TextureSetPosition, 1, Point(0, 3)),
					  Animation::Instruction(Animation::Instruction::Type::TextureShow, 1),
					  Animation::Instruction(Animation::Instruction::Type::Delay, 2, Time::Measurement::ticks),
					  Animation::Instruction(Animation::Instruction::Type::TextureMove, 1, Point(0, 1)),
					  Animation::Instruction(Animation::Instruction::Type::Delay, 2, Time::Measurement::ticks),
					  Animation::Instruction(Animation::Instruction::Type::TextureMove, 1, Point(0, 1)),
					  Animation::Instruction(Animation::Instruction::Type::Delay, 2, Time::Measurement::ticks),
					  Animation::Instruction(Animation::Instruction::Type::TextureHide, 1),
				  }
			  ),
			  m_callbackGroup(engine, true)
		{
			Output::Log("<Character::Character()> Start of function...", RGBColors::red);
			m_textures.resize(2);
			m_textures[0].Write({" O ", "/|\\", "/ \\"}, RGBAColors::yellow, RGBAColors::transparent, Point(0, 0));
			m_textures[1].Simple(UPoint(3, 1), CellA{.b = RGBColors::white | 63, .c = '~', .f = RGBAColors::gray});
			m_textures[1].m_active = false;

			m_colliders.resize(2);
			m_colliders[0].ByTextureCharacter(m_textures[0], 1);
			m_colliders[1].Simple(UPoint(5, 5), 3, Point(-1, -1));

			Output::Log("<Character::Character()> Registering callbacks", RGBColors::red);
			m_callbackGroup.RegisterCallback("w", [this]() -> bool { return Jump(); });
			m_callbackGroup.RegisterCallback("W", [this]() -> bool { return Jump(); });
			m_callbackGroup.RegisterCallback(" ", [this]() -> bool { return Jump(); });
			m_callbackGroup.RegisterCallback(Keys::up, [this]() { return Jump(); });

			m_callbackGroup.RegisterCallback("d", [this]() -> bool { return Move(Point(1, 0)); });
			m_callbackGroup.RegisterCallback("D", [this]() -> bool { return Move(Point(1, 0)); });
			m_callbackGroup.RegisterCallback(Keys::right, [this]() { return Move(Point(1, 0)); });

			m_callbackGroup.RegisterCallback("a", [this]() -> bool { return Move(Point(-1, 0)); });
			m_callbackGroup.RegisterCallback("A", [this]() -> bool { return Move(Point(-1, 0)); });
			m_callbackGroup.RegisterCallback(Keys::left, [this]() { return Move(Point(-1, 0)); });

			m_callbackGroup.RegisterCallback("f", [this]() -> bool { return PushBoxToDifferentLayer(); });
			m_callbackGroup.RegisterCallback("F", [this]() -> bool { return PushBoxToDifferentLayer(); });

			Output::Log("<Character::Character()> Entering layer", RGBColors::red);
			EnterLayer(layer);

			m_cam.m_name = "character cam";

			Output::Log("<Character::Character()> End of function.", RGBColors::red);
		}

	protected:
		auto OnTick() -> bool final
		{
			bool changed = false;

			if (m_yVelocity > 0)
			{
				for (size_t i = 0; i < m_yVelocity; i++)
					changed = Move(Point(0, -1));
				m_yVelocity--;
			}
			else
			{
				m_onGround = !Move(Point(0, 1));
				if (!m_onGround)
					changed = true;
			}

			if (m_cam.m_pos != m_pos - Point(6, 6))
			{
				m_cam.m_pos = m_pos - Point(6, 6);
				changed = true;
			}

			return changed;
		}

		void OnOverlap(Point /*p_dir*/, size_t /*p_collider*/, ID<Object> p_otherObject, size_t p_otherCollider) final
		{
			if (engine.memory.objects[p_otherObject]->m_colliders[p_otherCollider].m_type == 2)
				m_box = p_otherObject;
		}

		void OnOverlapExit(
			Point /*p_dir*/, size_t /*p_collider*/, ID<Object> p_otherObject, size_t /*p_otherCollider*/
		) final
		{
			if (p_otherObject == m_box)
				m_box = nullID<Object>;
		}

		void OnOverlappedExit(
			Point /*p_dir*/, size_t /*p_collider*/, ID<Object> p_otherObject, size_t /*p_otherCollider*/
		) final
		{
			if (p_otherObject == m_box)
				m_box = nullID<Object>;
		}

	private:
		ID<Layer> m_voidLayer;
		static constexpr int jumpStrength = 5;
		bool m_onGround = false;
		int m_yVelocity = 0;
		ID<Object> m_box;
		Animation m_jumpAnimation;
		Input::CallbackGroup m_callbackGroup;

		auto Jump() -> bool
		{
			if (m_onGround)
			{
				m_yVelocity = jumpStrength;
				m_onGround = false;
				m_jumpAnimation.Stop();
				m_jumpAnimation.Play();
				return true;
			}
			return false;
		}

		auto PushBoxToDifferentLayer() -> bool
		{
			engine.output.Log("<Character::PushBoxToDifferentLayer()> Start of function...", RGBColors::red);
			if (engine.memory.objects.Exists(m_box))
			{
				engine.output.Log("<Character::PushBoxToDifferentLayer()> Moving object to voidLayer", RGBColors::red);
				engine.memory.objects[m_box]->EnterLayer(m_voidLayer);
				m_box = ID<Object>();
				return true;
			}
			return false;
		}
	};

	class GravityBox : public Object
	{
	public:
		GravityBox(Engine& engine, ID<Layer> layer, Point pos, size_t fallingSpeed = 1)
			: Object(engine, pos), fallingSpeed(fallingSpeed)
		{
			m_textures.resize(1);
			m_textures[0].Write(
				{"#-#", "|G|", "#-#"},
				{.r = 80, .g = 55, .b = 50, .a = 255},
				{.r = 190, .g = 140, .b = 115, .a = 255},
				{.x = 0, .y = 0}
			);

			m_colliders.resize(1);
			m_colliders[0].Simple(UPoint(3, 3), 2, Point(0, 0));

			EnterLayer(layer);
		}

	protected:
		auto OnTick() -> bool final
		{
			// If caught, do not fall.
			bool moved = false;
			for (size_t i = 0; i < fallingSpeed; i++)
				if (Move(Point(0, 1)))
					moved = true;
			return moved;
		}

	private:
		bool onGround = false;
		size_t fallingSpeed;
	};

	class AutoUpdatingText : public Object
	{
	public:
		AutoUpdatingText(Engine& engine, float* data, Point pos, const ID<Layer>& layer, const std::string& text)
			: Object(engine, pos), m_data(data)
		{
			EnterLayer(layer);
			m_textures.resize(2);
			m_textures[0].Write({text}, RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 127), Point(0, 0));
		}

	protected:
		auto OnTick() -> bool final
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
				m_textures[1].Write(
					{std::to_string(averageData)},
					RGBA(255, 255, 255, 255),
					RGBA(0, 0, 0, 127),
					Point(m_textures[0].m_size.x, 0)
				);
				return true;
			}
			else
			{
				m_historyRotation++;
				return false;
			}
		}

	private:
		float* m_data;
		static constexpr size_t historyLength = 10;
		float m_dataHistory[historyLength];
		size_t m_historyRotation = 0;
	};

	struct QuitUI : UI
	{
		Button m_quitButton;

		Input::CallbackGroup callbackGroup;

		auto Toggle() -> bool
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
			: UI(engine, viewport),
			  m_quitButton(
				  engine,
				  m_id,
				  [ObjectPtr = &engine]() -> void { ObjectPtr->Quit(); },
				  Keys::return_,
				  Point(viewport.x / 2 - 3, viewport.y / 2 - 2),
				  "Quit",
				  true
			  ),
			  callbackGroup(engine, true)
		{
			AddWidget(m_quitButton.m_id);
			m_quitButton.Hide();
			m_quitButton.Deselect();
			callbackGroup.RegisterCallback(Keys::escape, std::bind(&QuitUI::Toggle, this));
		}
	};
}

auto main() -> int
{
	Engine engine(viewport, 24);
	bool charCamOn = false;

	// It would be preferable to have the startup notice drawn within the engine's image, but this is temporary.
	// engine.output.PrintStartupNotice("simpleplatform, a platform game example based on KTech.", "2023", "Ethan Kaufman (AKA Kaup)", "simpleplatform");

	Output::Log("<main()> Creating collider types", RGBColors::blue);
	engine.collision.colliderTypes = {
		{Collision::CR::B, Collision::CR::P, Collision::CR::P, Collision::CR::O}, // Heavy - 0
		{Collision::CR::B, Collision::CR::P, Collision::CR::P, Collision::CR::O}, // Normal - 1
		{Collision::CR::B, Collision::CR::B, Collision::CR::P, Collision::CR::O}, // Light - 2
		{Collision::CR::O, Collision::CR::O, Collision::CR::O, Collision::CR::O}  // Overlappable - 3
	};

	Output::Log("<main()> Creating map", RGBColors::blue);
	Map map(engine);
	map.m_name = "map";

	Output::Log("<main()> Creating layer", RGBColors::blue);
	Layer layer(engine, map.m_id);
	layer.m_name = "layer";
	Output::Log("<main()> Creating voidLayer", RGBColors::blue);
	Layer voidLayer(engine, map.m_id);
	voidLayer.m_name = "voidLayer";

	Output::Log("<main()> Creating camera", RGBColors::blue);
	Camera camera(engine, Point(0, 0), viewport);
	camera.m_name = "camera";
	Output::Log("<main()> Adding camera to map", RGBColors::blue);
	// `Camera` to `ID<Camera>` cast overload
	map.AddCamera(camera.m_id);

	Object worldProps(engine, Point(1, 1), "worldProps");
	worldProps.m_name = "worldProps";
	worldProps.m_textures.resize(3);
	Output::Log("<main()> Loading examples/simpleplatform/assets/sky.ktecht", RGBColors::blue);
	worldProps.m_textures[0].File("examples/simpleplatform/assets/sky.ktecht", Point(0, 0));
	Output::Log("<main()> Loading examples/simpleplatform/assets/house.ktecht", RGBColors::blue);
	worldProps.m_textures[1].File("examples/simpleplatform/assets/house.ktecht", Point(18, 14));
	Output::Log("<main()> Loading examples/simpleplatform/assets/land.ktecht", RGBColors::blue);
	worldProps.m_textures[2].File("examples/simpleplatform/assets/land.ktecht", Point(0, 3));
	worldProps.m_colliders.resize(1);
	worldProps.m_colliders[0].ByTextureBackground(worldProps.m_textures[2], 0, 1);
	layer.AddObject(worldProps.m_id);

	Output::Log("<main()> Creating frame", RGBColors::blue);
	Object frame(engine, Point(0, 0), "frame");
	frame.m_name = "frame";
	frame.m_textures.resize(5);
	frame.m_textures[0].Simple(
		UPoint{.x = 1, .y = viewport.y},
		CellA{.b = {.r = 255, .g = 255, .b = 255, .a = 255}, .c = '|', .f = {.r = 0, .g = 0, .b = 0, .a = 255}},
		Point(0, 0)
	);
	frame.m_textures[1].Simple(
		UPoint(1, viewport.y),
		CellA{.b = {.r = 255, .g = 255, .b = 255, .a = 255}, .c = '|', .f = {.r = 0, .g = 0, .b = 0, .a = 255}},
		Point(viewport.x - 1, 0)
	);
	frame.m_textures[2].Simple(
		UPoint(viewport.x, 1),
		CellA{.b = {.r = 255, .g = 255, .b = 255, .a = 255}, .c = '-', .f = {.r = 0, .g = 0, .b = 0, .a = 255}},
		Point(0, 0)
	);
	frame.m_textures[3].Simple(
		UPoint(viewport.x, 1),
		CellA{.b = {.r = 255, .g = 255, .b = 255, .a = 255}, .c = '-', .f = {.r = 0, .g = 0, .b = 0, .a = 255}},
		Point(0, viewport.y - 1)
	);
	frame.m_textures[4]
		.Write(
			{"'WASD'/'Arrow keys' to move.",
				"'Space' to jump.",
				"'F' to throw a gravity box off the layer.",
				"'M' to turn on following camera.",
				"'ESC' to toggle menu."},
			RGBAColors::black,
			RGBAColors::transparent,
			Point(2, 2)
		)
		.Transform([](CellA& cell) { cell.b = RGBA(255, 255, 255, 100); });
	frame.m_colliders.resize(4);
	frame.m_colliders[0].Simple(UPoint(viewport.x, 1), 0, Point(0, 0));
	frame.m_colliders[1].Simple(UPoint(1, viewport.y), 0, Point(0, 0));
	frame.m_colliders[2].Simple(UPoint(viewport.x, 1), 0, Point(0, viewport.y - 1));
	frame.m_colliders[3].Simple(UPoint(1, viewport.y), 0, Point(viewport.x - 1, 0));
	layer.AddObject(frame.m_id);

	Output::Log("<main()> Creating character", RGBColors::blue);
	Character character(engine, layer.m_id, voidLayer.m_id);
	character.m_name = "character";
	map.AddCamera(character.m_cam.m_id);
	Output::Log("<main()> Creating GravityBoxes", RGBColors::blue);
	GravityBox gbA(engine, layer.m_id, Point(10, 5), 1);
	gbA.m_name = "gbA";
	GravityBox gbB(engine, layer.m_id, Point(15, 5), 2);
	gbB.m_name = "gbB";
	GravityBox gbC(engine, layer.m_id, Point(30, 5), 3);
	gbC.m_name = "gbC";

	Output::Log("<main()> Creating house", RGBColors::blue);
	Object house(engine, Point(20, 32));
	house.m_name = "house";
	layer.AddObject(house.m_id);

	house.m_textures.resize(1);
	house.m_textures[0].File("examples/simpleplatform/assets/house.ktecht", {.x = 0, .y = 0});

	Input::CallbackGroup callbackGroup(engine, true);
	auto toggleCharacterCamera = [&engine, &charCamOn]() -> bool {
		charCamOn = !charCamOn;
		engine.output.Clear();
		return true;
	};
	callbackGroup.RegisterCallback("m", toggleCharacterCamera);
	callbackGroup.RegisterCallback("M", toggleCharacterCamera);

	Output::Log("<main()> Creating darkLayer", RGBColors::blue);
	Layer darkLayer(engine, map.m_id);
	darkLayer.m_name = "darkLayer";
	darkLayer.m_alpha = 127;

	Output::Log("<main()> Creating AutoUpdatingText", RGBColors::blue);
	AutoUpdatingText tpsText(engine, &engine.time.tpsPotential, Point(2, 27), layer.m_id, "TPS: ");
	tpsText.m_name = "tpsText";

	QuitUI quitUI(engine);

	Output::Log("<main()> Entering game loop", RGBColors::blue);
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
				camera.Render({layer.m_id, voidLayer.m_id, darkLayer.m_id});
				engine.output.Draw(camera.m_image, camera.m_res, Point(0, 0), UPoint(0, 0), UPoint(0, 0), 100);
				character.m_cam.Render(map.m_layers);
				engine.output.Draw(
					character.m_cam.m_image,
					character.m_cam.m_res,
					Point(
						18 - (character.m_cam.m_pos.x <= 0 ? character.m_cam.m_pos.x - 1 : 0),
						9 - (character.m_cam.m_pos.y <= 0 ? character.m_cam.m_pos.y - 1 : 0)
					),
					UPoint(
						character.m_cam.m_pos.x <= 0 ? -character.m_cam.m_pos.x + 1 : 0,
						character.m_cam.m_pos.y <= 0 ? -character.m_cam.m_pos.y + 1 : 0
					),
					UPoint(
						character.m_cam.m_pos.x + character.m_cam.m_res.x >= viewport.x
							? viewport.x - character.m_cam.m_pos.x - 1
							: 0,
						character.m_cam.m_pos.y + character.m_cam.m_res.y >= viewport.y
							? viewport.y - character.m_cam.m_pos.y - 1
							: 0
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
		// Output::Log("<main()> WaitUntilNextTick()", RGBColors::blue);
		engine.time.WaitUntilNextTick();
	}
	Output::Log("<main()> End of function.", RGBColors::blue);

	worldProps.m_textures[1].ExportToFile("examples/simpleplatform/assets/house.ktecht");
	worldProps.m_textures[2].ExportToFile("examples/simpleplatform/assets/land.ktecht");
	worldProps.m_textures[0].ExportToFile("examples/simpleplatform/assets/sky.ktecht");

	engine.output.outputOnQuit.push_back("character.m_pos.y: " + std::to_string(character.m_pos.y) + "\n");
}

// NOLINTEND(readability-*, bugprone-narrowing-conversions, bugprone-easily-swappable-parameters)
