/*
	simpleplatform1, a platform game example based on KTech.
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

using namespace KTech;

Engine engine(UPoint(50, 30), 24);

struct Character : Object
{
	size_t jumpSFX = engine.audio.CreateSource("assets/jump.wav");
	size_t groundHitSFX = engine.audio.CreateSource("assets/groundHit.wav");

	KTech::Layer* voidLayer;

	static constexpr int jumpStreng = 5;

	KTech::Camera cam;
	bool onGround = false;
	int yVelocity = 0;

	KTech::Object* box = nullptr;

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
	
	virtual void OnOverlap(Point dir, size_t collider, Object* otherObject, size_t otherCollider)
	{
		if (otherObject->colliders[otherCollider].type == 2)
			box = otherObject;
	}

	virtual void OnOverlapExit(Point dir, size_t collider, Object* otherObject, size_t otherCollider)
	{
		if (otherObject == box)
			box = nullptr;
	}
	virtual void OnOverlappedExit(Point dir, size_t collider, Object* otherObject, size_t otherCollider)
	{
		if (otherObject == box)
			box = nullptr;
	}

	void PushBoxToDifferentLayer()
	{
		if (box != nullptr) // nullptr for seemingly no reason
		{
			box->EnterLayer(voidLayer);
			box = nullptr;
		}
	}

	Character(KTech::Layer* layer, KTech::Layer* voidLayer) : Object(Point(5, 2), layer, "character"), voidLayer(voidLayer)
	{
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

		layer->parentMap->parentEngine->io.RegisterCallback("w", std::bind(&Character::Jump, this), true);
		layer->parentMap->parentEngine->io.RegisterCallback("W", std::bind(&Character::Jump, this), true);
		layer->parentMap->parentEngine->io.RegisterCallback(" ", std::bind(&Character::Jump, this), true);
		layer->parentMap->parentEngine->io.RegisterCallback(KTech::Keys::up, std::bind(&Character::Jump, this), true);

		layer->parentMap->parentEngine->io.RegisterCallback("d", std::bind(&Character::Right, this), true);
		layer->parentMap->parentEngine->io.RegisterCallback("D", std::bind(&Character::Right, this), true);
		layer->parentMap->parentEngine->io.RegisterCallback(KTech::Keys::right, std::bind(&Character::Right, this), true);

		layer->parentMap->parentEngine->io.RegisterCallback("a", std::bind(&Character::Left, this), true);
		layer->parentMap->parentEngine->io.RegisterCallback("A", std::bind(&Character::Left, this), true);
		layer->parentMap->parentEngine->io.RegisterCallback(KTech::Keys::left, std::bind(&Character::Left, this), true);

		layer->parentMap->parentEngine->io.RegisterCallback("f", std::bind(&Character::PushBoxToDifferentLayer, this), true);
		layer->parentMap->parentEngine->io.RegisterCallback("F", std::bind(&Character::PushBoxToDifferentLayer, this), true);

		cam = KTech::Camera(KTech::Point( 0, 0 ), KTech::UPoint( 15, 15 ));
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

	GravityBox(KTech::Layer* layer, KTech::Point pos, unsigned fallingSpeed = 1U) : Object(pos, layer), fallingSpeed(fallingSpeed)
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
	}
};

struct AutoUpdatingText : Object
{
	float* data;

	void OnTick()
	{
		textures[0].Write({std::to_string(*data)}, {255, 255, 255, 255}, {0, 0, 0, 127}, {0, 0});
	}

	AutoUpdatingText(float* data, KTech::Point pos, KTech::Layer* layer) : Object(pos, layer), data(data)
	{
		textures.resize(1);
	}
};

bool charCamOn = false;

void TurnOnCharacterCamera() {
	charCamOn = !charCamOn;
}

int main()
{
	using namespace KTech;

	// It would be preferable to have the startup notice drawn within the engine's image, but this is temporary.
	engine.io.PrintStartupNotice("simpleplatform1, a platform game example based on KTech.", "2023", "E. Kaufman (AKA Kaup)", "simpleplatform1");

	engine.collision.colliderTypes = { // For "simpleplatform1" example
		{ CR::B, CR::P, CR::P, CR::O }, // Heavy - 0
		{ CR::B, CR::P, CR::P, CR::O }, // Normal - 1
		{ CR::B, CR::B, CR::P, CR::O }, // Light - 2
		{ CR::O, CR::O, CR::O, CR::O } // Overlappable - 3
	};

	KTech::Map map(&engine);

	KTech::Layer layer(&map);
	KTech::Layer voidLayer(&map);

	KTech::Camera camera({ 0, 0 }, { 50, 30 });
	map.AddCamera(&camera, true);

	KTech::Object worldProps({ 1, 1 }, &layer, "worldProps");
	worldProps.textures.resize(3);
	KTech::IO::Log("<Main> Loading assets/sky.ktecht", RGBColors::blue);
	worldProps.textures[0].File("assets/sky.ktecht", { 0, 0 });
	KTech::IO::Log("<Main> Loading assets/land.ktecht", RGBColors::blue);
	worldProps.textures[2].File("assets/land.ktecht", { 0, 3 });
	
	worldProps.colliders.resize(1);
	worldProps.colliders[0].ByTextureBackground(worldProps.textures[2], 100, 0);
	uint8_t base = 29;

	KTech::Object frame(KTech::Point(0, 0), &layer, "frame");
	frame.textures.resize(5);
	frame.textures[0].Simple(KTech::UPoint(1, 30), KTech::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, 0));
	frame.textures[1].Simple(KTech::UPoint(1, 30), KTech::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(49, 0));
	frame.textures[2].Simple(KTech::UPoint(50, 1), KTech::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, 0));
	frame.textures[3].Simple(KTech::UPoint(50, 1), KTech::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), KTech::Point(0, 29));
	frame.textures[4].Write(
		{
			"'WASD'/'Arrow keys' to move.",
			"'Space' to jump.",
			"'f' to throw a gravity box off the layer.",
			"'m' to turn on following camera."
		}, RGBAColors::black, RGBAColors::transparent, Point(2, 2));
	frame.textures[4].Resize(frame.textures[4].GetSize(), CellA());
	frame.textures[4].SetBackground(RGBA(255, 255, 255, 100));
	frame.colliders.resize(4);
	frame.colliders[0].Simple(KTech::UPoint(50, 1), 0, KTech::Point(0, 0));
	frame.colliders[1].Simple(KTech::UPoint(1, 30), 0, KTech::Point(0, 0));
	frame.colliders[2].Simple(KTech::UPoint(50, 1), 0, KTech::Point(0, 29));
	frame.colliders[3].Simple(KTech::UPoint(1, 30), 0, KTech::Point(49, 0));

	Character character(&layer, &voidLayer);
	map.AddCamera(&character.cam);

	GravityBox gbA(&layer, {10, 5}, 1);
	GravityBox gbB(&layer, {15, 5}, 2);
	GravityBox gbC(&layer, {30, 5}, 3);

	KTech::Object house({ 20, 32 }, &layer, "");

	house.textures.resize(1);
	house.textures[0].File("assets/house.ktecht", { 0, 0 });

	engine.io.RegisterCallback("m", TurnOnCharacterCamera);
	engine.io.RegisterCallback("M", TurnOnCharacterCamera);

	KTech::Layer darkLayer(&map);
	darkLayer.alpha = 127;
	
	AutoUpdatingText audioPerformance(&engine.time.tpsPotential, Point(2, 27), &layer);

	engine.time.tpsLimit = 24;

	while (engine.running)
	{
		KTech::IO::Log("engine-loop-iteration start", RGBColors::blue);
		
		KTech::IO::Log("engine-loop-iteration StartThisTick", RGBColors::yellow);
		engine.time.StartThisTick();
		KTech::IO::Log("engine-loop-iteration Call", RGBColors::yellow);
		engine.io.Call();
		KTech::IO::Log("engine-loop-iteration CallOnTicks", RGBColors::yellow);
		map.CallOnTicks();

		if (map.activeCameraI != -1 && map.activeCameraI < map.cameras.size())
		{
			// continue;
			if (charCamOn) {
				map.cameras[0]->Render({ &layer, &darkLayer });
				engine.io.Draw(map.cameras[0]->image, Point(0, 0), 0, 0, 0, 0);
				map.cameras[1]->Render(map.layers);
				engine.io.Draw(map.cameras[1]->image, Point(18, 9), 0, 0, 0, 0);
			}
			else {
				KTech::IO::Log("engine-loop-iteration Render", RGBColors::yellow);
				map.cameras[0]->Render({ &layer });
				KTech::IO::Log("engine-loop-iteration Draw", RGBColors::yellow);
				engine.io.Draw(map.cameras[0]->image, Point(0, 0), 0, 0, 0, 0);
			}
			KTech::IO::Log("engine-loop-iteration Print", RGBColors::yellow);
			engine.io.Print();
		}
		// std::cout << "n="<< KTech::totalTicks << " | delta=" << KTech::deltaTime << " | fps=" << KTech::fps << " | pfps=" << KTech::potentialfps << std::endl;
		// std::cout << "x=" << KTech::terminalSize.ws_col << ", y=" << KTech::terminalSize.ws_row << std::endl;
		KTech::IO::Log("engine-loop-iteration WaitUntilNextTick", RGBColors::yellow);
		engine.time.WaitUntilNextTick();

		KTech::IO::Log("engine-loop-iteration end", RGBColors::green);
	}
}
