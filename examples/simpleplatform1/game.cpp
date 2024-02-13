/*
	simpleplatform1, a platform game example based on KTech.
	Copyright (C) 2023 Ethan Kaufman (AKA Kaup)

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

struct Character : Object
{
	size_t jumpSFX = engine.audio.CreateSource("assets/jump.wav");
	size_t groundHitSFX = engine.audio.CreateSource("assets/groundHit.wav");

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
			engine.audio.PlaySource(jumpSFX);
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

	virtual void OnTick()
	{
		IO::Log("<Character::OnTick> Start of function...", RGBColors::pink);

		bool left = false;
		bool right = false;
		
		bool priorOnGround = onGround;

		IO::Log("<Character::OnTick> Moving", RGBColors::pink);

		if (yVelocity > 0)
		{
			for (size_t i = 0; i < yVelocity; i++)
				Move(Point(0, -1));
			yVelocity--;
		}
		else
			onGround = !Move(Point(0, 1));

		IO::Log("<Character::OnTick> Moving camera", RGBColors::pink);

		cam.pos = { pos.x - 6, pos.y - 6 };
	
		if (!priorOnGround && onGround)
		 	engine.audio.PlaySource(groundHitSFX, 0, 0, 0.5f);

		IO::Log("<Character::OnTick> End of function.", RGBColors::pink);
	}
	
	virtual void OnOverlap(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider)
	{
		if (engine.memory.objects[otherObject]->colliders[otherCollider].type == 2)
			box = otherObject;
	}

	virtual void OnOverlapExit(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider)
	{
		if (otherObject == box)
			box = ID<Object>(0, 0);
	}
	virtual void OnOverlappedExit(Point dir, size_t collider, ID<Object> otherObject, size_t otherCollider)
	{
		if (otherObject == box)
			box = ID<Object>(0, 0);
	}

	void PushBoxToDifferentLayer()
	{
		engine.io.Log("<Character::PushBoxToDifferentLayer()> Start of function...", RGBColors::red);
		if (engine.memory.objects.Exists(box))
		{
			engine.io.Log("<Character::PushBoxToDifferentLayer()> Moving object to voidLayer", RGBColors::red);
			engine.memory.objects[box]->EnterLayer(voidLayer);
			box = ID<Object>(0, 0);
		}
	}

	Character(Engine& engine, ID<Layer>& layer, ID<Layer>& voidLayer)
		: Object(engine, Point(5, 2), "character"), voidLayer(voidLayer), cam(engine, KTech::Point( 0, 0 ), KTech::UPoint( 15, 15 ))
	{
		KTech::IO::Log("<Character::Character()> Start of function...", RGBColors::red);
		textures.resize(1);
		textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 255, 255, 0, 255 }, { 0, 0, 0, 0 }, { 0, 0 }
		);
		colliders.resize(2);
		colliders[0].ByTextureCharacter(textures[0], 100, 1);
		colliders[1].Simple(KTech::UPoint(5, 5), 3, KTech::Point(-1, -1));

		KTech::IO::Log("<Character::Character()> Registering callbacks", RGBColors::red);
		engine.io.RegisterCallback("w", std::bind(&Character::Jump, this), true);
		engine.io.RegisterCallback("W", std::bind(&Character::Jump, this), true);
		engine.io.RegisterCallback(" ", std::bind(&Character::Jump, this), true);
		engine.io.RegisterCallback(KTech::Keys::up, std::bind(&Character::Jump, this), true);

		engine.io.RegisterCallback("d", std::bind(&Character::Right, this), true);
		engine.io.RegisterCallback("D", std::bind(&Character::Right, this), true);
		engine.io.RegisterCallback(KTech::Keys::right, std::bind(&Character::Right, this), true);

		engine.io.RegisterCallback("a", std::bind(&Character::Left, this), true);
		engine.io.RegisterCallback("A", std::bind(&Character::Left, this), true);
		engine.io.RegisterCallback(KTech::Keys::left, std::bind(&Character::Left, this), true);

		engine.io.RegisterCallback("f", std::bind(&Character::PushBoxToDifferentLayer, this), true);
		engine.io.RegisterCallback("F", std::bind(&Character::PushBoxToDifferentLayer, this), true);

		KTech::IO::Log("<Character::Character()> Entering layer", RGBColors::red);
		EnterLayer(layer);

		cam.name = "character cam";

		KTech::IO::Log("<Character::Character()> End of function.", RGBColors::red);
	}
};

struct GravityBox : Object
{
	bool onGround = false;

	unsigned fallingSpeed;

	void OnTick()
	{
		// If catched, do not fall.
		for (size_t i = 0; i < fallingSpeed; i++)
			Move(Point(0, 1));
	}

	GravityBox(Engine& engine, KTech::ID<Layer> layer, KTech::Point pos, unsigned fallingSpeed = 1U) : Object(engine, pos), fallingSpeed(fallingSpeed)
	{
		textures.resize(1);
		textures[0].Write(
			{
				"#-#",
				"|G|",
				"#-#"
			}, { 100, 0, 200, 255 }, { 200, 200, 200, 200 }, { 0, 0 }
		);

		colliders.resize(1);
		colliders[0].Simple(KTech::UPoint(3, 3), 2, KTech::Point(0, 0));
	
		EnterLayer(layer);
	}
};

struct AutoUpdatingText : Object
{
	float* data;

	void OnTick()
	{
		textures[0].Write({std::to_string(*data)}, {255, 255, 255, 255}, {0, 0, 0, 127}, {0, 0});
	}

	AutoUpdatingText(Engine& engine, float* data, KTech::Point pos, ID<Layer>& layer) : Object(engine, pos), data(data)
	{
		EnterLayer(layer);
		textures.resize(1);
	}
};

bool charCamOn = false;

void TurnOnCharacterCamera() {
	charCamOn = !charCamOn;
}

Engine engine(UPoint(50, 30), 24);

int main()
{
	using namespace KTech;

	// It would be preferable to have the startup notice drawn within the engine's image, but this is temporary.
	// engine.io.PrintStartupNotice("simpleplatform1, a platform game example based on KTech.", "2023", "Ethan Kaufman (AKA Kaup)", "simpleplatform1");

	KTech::IO::Log("<main()> Creating collider types", RGBColors::blue);
	engine.collision.colliderTypes = { // For "simpleplatform1" example
		{ CR::B, CR::P, CR::P, CR::O }, // Heavy - 0
		{ CR::B, CR::P, CR::P, CR::O }, // Normal - 1
		{ CR::B, CR::B, CR::P, CR::O }, // Light - 2
		{ CR::O, CR::O, CR::O, CR::O } // Overlappable - 3
	};

	KTech::IO::Log("<main()> Creating map", RGBColors::blue);
	KTech::Map map(engine);
	map.name = "map";

	KTech::IO::Log("<main()> Creating layer", RGBColors::blue);
	KTech::Layer layer(engine, map.id);
	layer.name = "layer";
	KTech::IO::Log("<main()> Creating voidLayer", RGBColors::blue);
	KTech::Layer voidLayer(engine, map.id);
	voidLayer.name = "voidLayer";

	KTech::IO::Log("<main()> Creating camera", RGBColors::blue);
	KTech::Camera camera(engine, Point(0, 0), UPoint(50, 30));
	camera.name = "camera";
	KTech::IO::Log("<main()> Adding camera to map", RGBColors::blue);
	// `Camera` to `ID<Camera>` cast overload
	map.AddCamera(camera.id, true);

	KTech::Object worldProps(engine, Point(1, 1), "worldProps");
	worldProps.name = "worldProps";
	worldProps.textures.resize(3);
	KTech::IO::Log("<main()> Loading assets/sky.ktecht", RGBColors::blue);
	worldProps.textures[0].File("assets/sky.ktecht", { 0, 0 });
	KTech::IO::Log("<main()> Loading assets/house.ktecht", RGBColors::blue);
	worldProps.textures[1].File("assets/house.ktecht", Point(18, 14));
	KTech::IO::Log("<main()> Loading assets/land.ktecht", RGBColors::blue);
	worldProps.textures[2].File("assets/land.ktecht", { 0, 3 });
	worldProps.colliders.resize(1);
	worldProps.colliders[0].ByTextureBackground(worldProps.textures[2], 100, 0);
	layer.AddObject(worldProps.id);

	KTech::IO::Log("<main()> Creating frame", RGBColors::blue);
	KTech::Object frame(engine, KTech::Point(0, 0), "frame");
	frame.name = "frame";
	frame.textures.resize(5);
	frame.textures[0].Simple(KTech::UPoint(1, 30), KTech::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, 0));
	frame.textures[1].Simple(KTech::UPoint(1, 30), KTech::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(49, 0));
	frame.textures[2].Simple(KTech::UPoint(50, 1), KTech::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, 0));
	frame.textures[3].Simple(KTech::UPoint(50, 1), KTech::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, 29));
	frame.textures[4].Write(
		{
			"'WASD'/'Arrow keys' to move.",
			"'Space' to jump.",
			"'F' to throw a gravity box off the layer.",
			"'M' to turn on following camera."
		}, RGBAColors::black, RGBAColors::transparent, Point(2, 2));
	frame.textures[4].Resize(frame.textures[4].GetSize(), CellA());
	frame.textures[4].SetBackground(RGBA(255, 255, 255, 100));
	frame.colliders.resize(4);
	frame.colliders[0].Simple(KTech::UPoint(50, 1), 0, KTech::Point(0, 0));
	frame.colliders[1].Simple(KTech::UPoint(1, 30), 0, KTech::Point(0, 0));
	frame.colliders[2].Simple(KTech::UPoint(50, 1), 0, KTech::Point(0, 29));
	frame.colliders[3].Simple(KTech::UPoint(1, 30), 0, KTech::Point(49, 0));
	layer.AddObject(frame.id);

	KTech::IO::Log("<main()> Creating character", RGBColors::blue);
	Character character(engine, layer.id, voidLayer.id);
	character.name = "character";
	map.AddCamera(character.cam.id);

	KTech::IO::Log("<main()> Creating GravityBoxes", RGBColors::blue);
	GravityBox gbA(engine, layer.id, Point(10, 5), 1);
	gbA.name = "gbA";
	GravityBox gbB(engine, layer.id, Point(15, 5), 2);
	gbB.name = "gbB";
	GravityBox gbC(engine, layer.id, Point(30, 5), 3);
	gbC.name = "gbC";

	KTech::IO::Log("<main()> Creating house", RGBColors::blue);
	KTech::Object house(engine, Point(20, 32));
	house.name = "house";
	layer.AddObject(house.id);

	house.textures.resize(1);
	house.textures[0].File("assets/house.ktecht", { 0, 0 });

	engine.io.RegisterCallback("m", TurnOnCharacterCamera);
	engine.io.RegisterCallback("M", TurnOnCharacterCamera);

	KTech::IO::Log("<main()> Creating darkLayer", RGBColors::blue);
	KTech::Layer darkLayer(engine, map.id);
	darkLayer.name = "darkLayer";
	darkLayer.alpha = 127;
	
	KTech::IO::Log("<main()> Creating AutoUpdatingText", RGBColors::blue);
	AutoUpdatingText tpsText(engine, &engine.time.tpsPotential, Point(2, 27), layer.id);
	tpsText.name = "tpsText";

	KTech::IO::Log("<main()> Entering game loop", RGBColors::blue);
	while (engine.running)
	{
		KTech::IO::Log("<main()> StartOfThisTick()", RGBColors::blue);
		engine.time.StartThisTick();
		KTech::IO::Log("<main()> io.Call()", RGBColors::blue);
		engine.io.Call();
		KTech::IO::Log("<main()> CallOnTicks()", RGBColors::blue);
		engine.memory.CallOnTicks();

		KTech::IO::Log("<main()> Render and print", RGBColors::blue);
		if (map.activeCameraI != -1 && map.activeCameraI < map.cameras.size())
		{
			if (charCamOn) {
				camera.Render({ layer.id, voidLayer.id, darkLayer.id });
				// darkLayer is the dark post processing effect
				engine.io.Draw(camera.image, Point(0, 0), 0, 0, 0, 0);
				character.cam.Render(map.layers);
				engine.io.Draw(character.cam.image, Point(18, 9), 0, 0, 0, 0);
			}
			else {
				camera.Render(map.layers);
				engine.io.Draw(camera.image, Point(0, 0), 0, 0, 0, 0);
			}
			engine.io.Print();
		}
		// KTech::IO::Log("<main()> WaitUntilNextTick()", RGBColors::blue);
		engine.time.WaitUntilNextTick();
	}
	KTech::IO::Log("<main()> End of function.", RGBColors::blue);
}