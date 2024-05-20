/*
	simpleplatform, a platform game example based on KTech.
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

using namespace KTech;

static constexpr UPoint viewport = UPoint(50, 30);

Engine engine(viewport, 24);

struct Character : Object
{
	KTech::ID<Layer> voidLayer;

	static constexpr int jumpStreng = 5;

	Camera cam;
	bool onGround = false;
	int yVelocity = 0;

	ID<Object> box;

	void Jump()
	{
		if (onGround) {
			yVelocity = jumpStreng;
			onGround = false;
		}
	}

	void Right()
	{
		Move({ 1 });
	}

	void Left()
	{
		Move({ -1 });
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

	void PushBoxToDifferentLayer()
	{
		engine.output.Log("<Character::PushBoxToDifferentLayer()> Start of function...", RGBColors::red);
		if (engine.memory.objects.Exists(box))
		{
			engine.output.Log("<Character::PushBoxToDifferentLayer()> Moving object to voidLayer", RGBColors::red);
			engine.memory.objects[box]->EnterLayer(voidLayer);
			box = ID<Object>(0, 0);
		}
	}

	Character(Engine& engine, ID<Layer>& layer, ID<Layer>& voidLayer)
		: Object(engine, Point(5, 2), "character"), voidLayer(voidLayer), cam(engine, KTech::Point( 0, 0 ), KTech::UPoint( 15, 15 ))
	{
		KTech::Output::Log("<Character::Character()> Start of function...", RGBColors::red);
		m_textures.resize(1);
		m_textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 255, 255, 0, 255 }, { 0, 0, 0, 0 }, { 0, 0 }
		);
		m_colliders.resize(2);
		m_colliders[0].ByTextureCharacter(m_textures[0], 100, 1);
		m_colliders[1].Simple(KTech::UPoint(5, 5), 3, KTech::Point(-1, -1));

		KTech::Output::Log("<Character::Character()> Registering callbacks", RGBColors::red);
		engine.input.RegisterCallback("w", std::bind(&Character::Jump, this), true);
		engine.input.RegisterCallback("W", std::bind(&Character::Jump, this), true);
		engine.input.RegisterCallback(" ", std::bind(&Character::Jump, this), true);
		engine.input.RegisterCallback(KTech::Keys::up, std::bind(&Character::Jump, this), true);

		engine.input.RegisterCallback("d", std::bind(&Character::Right, this), true);
		engine.input.RegisterCallback("D", std::bind(&Character::Right, this), true);
		engine.input.RegisterCallback(KTech::Keys::right, std::bind(&Character::Right, this), true);

		engine.input.RegisterCallback("a", std::bind(&Character::Left, this), true);
		engine.input.RegisterCallback("A", std::bind(&Character::Left, this), true);
		engine.input.RegisterCallback(KTech::Keys::left, std::bind(&Character::Left, this), true);

		engine.input.RegisterCallback("f", std::bind(&Character::PushBoxToDifferentLayer, this), true);
		engine.input.RegisterCallback("F", std::bind(&Character::PushBoxToDifferentLayer, this), true);

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
		// If catched, do not fall.
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

	AutoUpdatingText(Engine& engine, float* data, KTech::Point pos, ID<Layer>& layer, std::string text)
		: Object(engine, pos), m_data(data)
	{
		EnterLayer(layer);
		m_textures.resize(2);
		m_textures[0].Write({ text },  RGBA(255, 255, 255, 255), RGBA(0, 0, 0, 127), Point(0, 0));
	}
};

bool charCamOn = false;

void TurnOnCharacterCamera()
{
	charCamOn = !charCamOn;
	engine.output.Clear();
}

int main()
{
	using namespace KTech;

	// It would be preferable to have the startup notice drawn within the engine's image, but this is temporary.
	// engine.output.PrintStartupNotice("simpleplatform, a platform game example based on KTech.", "2023", "Ethan Kaufman (AKA Kaup)", "simpleplatform");

	KTech::Output::Log("<main()> Creating collider types", RGBColors::blue);
	engine.collision.colliderTypes = {
		{ CR::B, CR::P, CR::P, CR::O }, // Heavy - 0
		{ CR::B, CR::P, CR::P, CR::O }, // Normal - 1
		{ CR::B, CR::B, CR::P, CR::O }, // Light - 2
		{ CR::O, CR::O, CR::O, CR::O }  // Overlappable - 3
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
	map.AddCamera(camera.m_id, true);

	KTech::Object worldProps(engine, Point(1, 1), "worldProps");
	worldProps.m_name = "worldProps";
	worldProps.m_textures.resize(3);
	KTech::Output::Log("<main()> Loading assets/sky.ktecht", RGBColors::blue);
	worldProps.m_textures[0].File("assets/sky.ktecht", Point(0, 0));
	KTech::Output::Log("<main()> Loading assets/house.ktecht", RGBColors::blue);
	worldProps.m_textures[1].File("assets/house.ktecht", Point(18, 14));
	KTech::Output::Log("<main()> Loading assets/land.ktecht", RGBColors::blue);
	worldProps.m_textures[2].File("assets/land.ktecht", Point(0, 3));
	worldProps.m_colliders.resize(1);
	worldProps.m_colliders[0].ByTextureBackground(worldProps.m_textures[2], 100, 0);
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
			"'M' to turn on following camera."
		}, RGBAColors::black, RGBAColors::transparent, Point(2, 2));
	frame.m_textures[4].Resize(frame.m_textures[4].m_size, CellA());
	frame.m_textures[4].SetBackground(RGBA(255, 255, 255, 100));
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
	house.m_textures[0].File("assets/house.ktecht", { 0, 0 });

	engine.input.RegisterCallback("m", TurnOnCharacterCamera);
	engine.input.RegisterCallback("M", TurnOnCharacterCamera);

	KTech::Output::Log("<main()> Creating darkLayer", RGBColors::blue);
	KTech::Layer darkLayer(engine, map.m_id);
	darkLayer.m_name = "darkLayer";
	darkLayer.m_alpha = 127;
	
	KTech::Output::Log("<main()> Creating AutoUpdatingText", RGBColors::blue);
	AutoUpdatingText tpsText(engine, &engine.time.tpsPotential, Point(2, 27), layer.m_id, "TPS: ");
	tpsText.m_name = "tpsText";
	
	KTech::Output::Log("<main()> Entering game loop", RGBColors::blue);
	while (engine.running)
	{
		engine.input.CallHandlers();
		engine.memory.CallOnTicks();

		if (map.m_activeCameraI != -1 && map.m_activeCameraI < map.m_cameras.size() && engine.output.ShouldRenderThisTick())
		{
			if (charCamOn) 
			{
				engine.output.Clear(); // Allows to make foreground darker
				camera.Render({ layer.m_id, voidLayer.m_id, darkLayer.m_id });
				engine.output.Draw(camera.m_image, Point(0, 0), 0, 0, 0, 0, 127);
				character.cam.Render(map.m_layers);
				engine.output.Draw(
					character.cam.m_image,
					Point(
						18 - (character.cam.m_pos.x <= 0 ? character.cam.m_pos.x - 1 : 0),
						9 - (character.cam.m_pos.y <= 0 ? character.cam.m_pos.y - 1 : 0)),
					character.cam.m_pos.x <= 0 ? -character.cam.m_pos.x + 1 : 0,
					character.cam.m_pos.y <= 0 ? -character.cam.m_pos.y + 1 : 0,
					character.cam.m_pos.x + character.cam.m_res.x >= viewport.x ? viewport.x - character.cam.m_pos.x - 1 : 0,
					character.cam.m_pos.y + character.cam.m_res.y >= viewport.y ? viewport.y - character.cam.m_pos.y - 1 : 0
				);
			}
			else
			{
				camera.Render(map.m_layers);
				engine.output.Draw(camera.m_image, Point(0, 0), 0, 0, 0, 0);
			}
			engine.output.Print();
		}
		else if (engine.output.ShouldPrintThisTick())
			engine.output.Print();
		// KTech::Output::Log("<main()> WaitUntilNextTick()", RGBColors::blue);
		engine.time.WaitUntilNextTick();
	}
	KTech::Output::Log("<main()> End of function.", RGBColors::blue);
}