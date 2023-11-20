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

bool catchingCharacterB = false;

Engine::Map* pmap;

struct Character
{
	Engine::AudioSource jumpSFX = Engine::AudioSource("assets/jump.wav");
	Engine::AudioSource groundHitSFX = Engine::AudioSource("assets/groundHit.wav");

	Engine::Layer* voidLayer;

	// Object* last

	static constexpr int jumpStreng = 6;

	Engine::Object obj;
	Engine::Camera cam;
	bool onGround = false;
	int yVelocity = 0;

	Engine::Object* box = nullptr;

	void Jump()
	{
		if (onGround) {
			yVelocity -= jumpStreng;
			jumpSFX.Play(0, 0, 0.7f);
			onGround = false;
		}
	}

	void Right()
	{
		obj.Move({ 1 });
	}

	void Left()
	{
		obj.Move({ -1 });
	}

	void OnEvent(Engine::Object::EventType eventType)
	{
		switch (eventType)
		{
			case Engine::Object::EventType::onTick:
			{
				if (yVelocity < 1) {
					yVelocity++;
				}

				bool left = false; // IsKeyDown(VK_LEFT);
				bool right = false; // IsKeyDown(VK_RIGHT);
				
				bool priorOnGround = onGround;

				if (yVelocity < 0)
					for (size_t i = 0; i < -yVelocity; i++)
						obj.Move({ 0, -1 });
				else if (yVelocity == 1)
					onGround = !obj.Move({ 0, 1 });

				cam.pos = { obj.pos.x - 6, obj.pos.y - 6 };

				if (!priorOnGround && onGround) groundHitSFX.Play(0, 0, 0.5f);
				break;
			}
			case Engine::Object::EventType::onOverlap:
			{
				if (obj.otherObject->colliders[obj.otherColliderIndex].type == 2)
					box = obj.otherObject;
				break;
			}
			case Engine::Object::EventType::onOverlapExit: case Engine::Object::EventType::onOverlappedExit:
			{
				if (obj.otherObject == box)
					box = nullptr;
				break;
			}
			default:
			{
				break;
			}
		}
		
	}

	void PushBoxToDifferentLayer()
	{
		if (box != nullptr) // nullptr for seemingly no reason
		{
			box->EnterLayer(voidLayer);
			box = nullptr;
		}
	}

	Character(Engine::Layer* layer, Engine::Layer* voidLayer) : voidLayer(voidLayer)
	{
		obj.pos = { 5, 2 };
		obj.textures.resize(1);
		obj.textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 255, 255, 0, 255 }, { 0, 0, 0, 0 }, { 0, 0 }
		);
		obj.colliders.resize(2);
		obj.colliders[0].ByTextureCharacter(obj.textures[0], 100, 1);
		obj.colliders[1].Simple(Engine::UPoint(5, 5), 3, Engine::Point(-1, -1));

		cam = Engine::Camera(Engine::Point( 0, 0 ), Engine::UPoint( 15, 15 ));

		Engine::Input::RegisterCallback("w", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterCallback("W", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterCallback(" ", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterCallback(Engine::Input::K::up, std::bind(&Character::Jump, this), true);

		Engine::Input::RegisterCallback("d", std::bind(&Character::Right, this), true);
		Engine::Input::RegisterCallback("D", std::bind(&Character::Right, this), true);
		Engine::Input::RegisterCallback(Engine::Input::K::right, std::bind(&Character::Right, this), true);

		Engine::Input::RegisterCallback("a", std::bind(&Character::Left, this), true);
		Engine::Input::RegisterCallback("A", std::bind(&Character::Left, this), true);
		Engine::Input::RegisterCallback(Engine::Input::K::left, std::bind(&Character::Left, this), true);

		Engine::Input::RegisterCallback("f", std::bind(&Character::PushBoxToDifferentLayer, this), true);
		Engine::Input::RegisterCallback("F", std::bind(&Character::PushBoxToDifferentLayer, this), true);

		obj.OnEvent = std::bind(&Character::OnEvent, this, std::placeholders::_1);
		obj.EnterLayer(layer);
	}
};

struct GravityBox
{
	Engine::Object obj;

	bool onGround = false;

	unsigned fallingSpeed;

	void OnEvent(Engine::Object::EventType eventType)
	{
		if (eventType != Engine::Object::EventType::onTick)
			return;

		// If catched, do not fall.
		for (size_t i = 0; i < fallingSpeed; i++)
			obj.Move({ 0, 1 });
	}

	GravityBox(Engine::Layer* layer, Engine::Point pos, unsigned fallingSpeed = 1U) : fallingSpeed(fallingSpeed)
	{
		obj.pos = pos;

		obj.textures.resize(1);
		obj.textures[0].Write(
			{
				"#-#",
				"|G|",
				"#-#"
			}, { 100, 0, 200, 255 }, { 200, 200, 200, 200 }, { 0, 0 }
		);

		obj.colliders.resize(1);
		obj.colliders[0].Simple(Engine::UPoint(3, 3), 2, Engine::Point(0, 0));

		obj.OnEvent = std::bind(&GravityBox::OnEvent, this, std::placeholders::_1);
		
		obj.EnterLayer(layer);
	}
};

struct AutoUpdatingText
{
	float* data;
	Engine::Object obj;

	void OnEvent(Engine::Object::EventType eventType)
	{
		if (eventType != Engine::Object::EventType::onTick)
			return;
		
		obj.textures[0].Write({std::to_string(*data)}, {255, 255, 255, 255}, {0, 0, 0, 127}, {0, 0});
	}

	AutoUpdatingText(float* data, Engine::Point pos, Engine::Layer* layer) : data(data)
	{
		obj.textures.resize(1);
		obj.pos = pos;
		obj.OnEvent = std::bind(&AutoUpdatingText::OnEvent, this, std::placeholders::_1);
		obj.EnterLayer(layer);
	}
};

bool charCamOn = false;

void TurnOnCharacterCamera() {
	charCamOn = !charCamOn;
}

int main()
{
	using namespace Engine;

	colliderTypes = { // For "simpleplatform1" example
		{ CR::B, CR::P, CR::P, CR::O }, // Heavy - 0
		{ CR::B, CR::P, CR::P, CR::O }, // Normal - 1
		{ CR::B, CR::B, CR::P, CR::O }, // Light - 2
		{ CR::O, CR::O, CR::O, CR::O } // Overlappable - 3
	};

	Engine::PrepareTerminal({50, 30});
	Engine::InitializeAudio();

	Engine::Map map;
	pmap = &map;

	Engine::Layer layer;
	Engine::Layer voidLayer;
	map.AddLayer(&layer);
	map.AddLayer(&voidLayer);

	Engine::Camera camera({ 0, 0 }, { 50, 30 });
	map.AddCamera(&camera, true);

	Engine::Object worldProps({ 1, 1 }, &layer);
	worldProps.textures.resize(3);
	Engine::Log("<Main> Loading assets/sky.ktecht", RGBColors::blue);
	worldProps.textures[0].File("assets/sky.ktecht", { 0, 0 });
	Engine::Log("<Main> Loading assets/land.ktecht", RGBColors::blue);
	worldProps.textures[2].File("assets/land.ktecht", { 0, 3 });
	
	worldProps.colliders.resize(1);
	worldProps.colliders[0].ByTextureBackground(worldProps.textures[2], 100, 0);
	uint8_t base = 29;

	Engine::Object frame(Engine::Point(0, 0), &layer, "L");
	frame.textures.resize(5);
	frame.textures[0].Simple(Engine::UPoint(1, 30), Engine::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), Engine::Point(0, 0));
	frame.textures[1].Simple(Engine::UPoint(1, 30), Engine::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), Engine::Point(49, 0));
	frame.textures[2].Simple(Engine::UPoint(50, 1), Engine::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), Engine::Point(0, 0));
	frame.textures[3].Simple(Engine::UPoint(50, 1), Engine::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), Engine::Point(0, 29));
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
	frame.colliders[0].Simple(Engine::UPoint(50, 1), 0, Engine::Point(0, 0));
	frame.colliders[1].Simple(Engine::UPoint(1, 30), 0, Engine::Point(0, 0));
	frame.colliders[2].Simple(Engine::UPoint(50, 1), 0, Engine::Point(0, 29));
	frame.colliders[3].Simple(Engine::UPoint(1, 30), 0, Engine::Point(49, 0));

	Character character(&layer, &voidLayer);
	map.AddCamera(&character.cam);

	GravityBox gbA(&layer, {10, 5}, 1);
	GravityBox gbB(&layer, {15, 5}, 2);
	GravityBox gbC(&layer, {30, 5}, 3);

	Engine::Object house({ 20, 32 }, &layer, "");

	house.textures.resize(1);
	Engine::Log("<Main> Loading house", RGBColors::blue);
	house.textures[0].File("assets/house.ktecht", { 0, 0 });

	Engine::Input::RegisterCallback("m", TurnOnCharacterCamera);
	Engine::Input::RegisterCallback("M", TurnOnCharacterCamera);

	Engine::Layer darkLayer;
	darkLayer.alpha = 127;
	
	AutoUpdatingText audioPerformance(&Engine::Time::tpsPotential, Point(2, 27), &layer);

	std::thread t_inputLoop(Engine::Input::Loop);

	Engine::Time::tpsLimit = 24;

	while (Engine::running)
	{
		Engine::Time::StartThisTick();

		Engine::Input::Call();
		map.CallOnTicks();

		if (map.activeCameraI != -1 && map.activeCameraI < map.cameras.size())
		{
			// continue;
			if (charCamOn) {
				map.cameras[0]->Render({ &layer, &darkLayer });
				map.cameras[0]->Draw({ 0, 0 }, 0, 0, 0, 0);
				map.cameras[1]->Render(map.layers);
				map.cameras[1]->Draw({ 18, 9 }, 0, 0, 0, 0);
			}
			else {
				map.cameras[0]->Render(map.layers);
				map.cameras[0]->Draw({ 0, 0 }, 0, 0, 0, 0);
			}
			Engine::Print();
		}
		// std::cout << "n="<< Engine::totalTicks << " | delta=" << Engine::deltaTime << " | fps=" << Engine::fps << " | pfps=" << Engine::potentialfps << std::endl;
		// std::cout << "x=" << Engine::terminalSize.ws_col << ", y=" << Engine::terminalSize.ws_row << std::endl;
		Engine::Time::WaitUntilNextTick();

		// running = false;
	}
	
	Engine::TerminateAudio();
	Engine::ResetTerminal();
	exit(0);
}