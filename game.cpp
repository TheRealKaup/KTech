#include "engine/engine.hpp"

bool catchingCharacterB = false;

Engine::Map* pmap;

static void Log(std::string string)
{
	std::cout << "\033[38;2;255;0;255m" << string << "\033[m" << std::endl << std::flush;
}

struct Character
{
	Engine::AudioSource jumpSFX;
	Engine::AudioSource groundHitSFX;

	const int jumpStreng = 4;

	Engine::Object obj;
	Engine::Camera cam;
	bool onGround = false;
	int yVelocity = 0;

	void Jump()
	{
		if (onGround) {
			yVelocity -= jumpStreng;
			jumpSFX.Play(0, 0, 0, 0.7f);
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

	void OnTick()
	{
		if (yVelocity < 1) {
			yVelocity++;
		}

		bool left = false; // IsKeyDown(VK_LEFT);
		bool right = false; // IsKeyDown(VK_RIGHT);
		
		bool priorOnGround = onGround;

		if (yVelocity < 0)
			for (size_t i = 0; i < -yVelocity; i++)
				onGround = !obj.Move({ 0, -1 });
		else if (yVelocity == 1)
			onGround = !obj.Move({ 0, 1 });

		cam.pos = { obj.pos.x - 6, obj.pos.y - 6 };

		// if (!priorOnGround && onGround) groundHitSFX.Play(0, 0, 0, 0.5f);
	}

	Character(Engine::Layer* layer)
	{
		groundHitSFX.LoadWavFile("Assets/groundHit2.wav");
		jumpSFX.LoadWavFile("Assets/jump.wav");

		obj.pos = { 5, 2 };//
		obj.textures.resize(1);
		obj.textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 0, 0, 0, 1.0f }, { 0, 0, 0, 0.0f }, { 0, 0 }
		);
		obj.colliders.push_back(Engine::Collider({ 1, 2 }, { 0, 1 }, 1));
		obj.colliders.push_back(Engine::Collider({ 1, 2 }, { 1, 0 }, 1));
		obj.colliders.push_back(Engine::Collider({ 1, 2 }, { 2, 1 }, 1));

		cam = Engine::Camera({ 0, 0 }, { 15, 15 });

		Engine::Input::RegisterHandler("w", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterHandler("W", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterHandler(" ", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterHandler("\033[A", std::bind(&Character::Jump, this), true);

		Engine::Input::RegisterHandler("d", std::bind(&Character::Right, this));
		Engine::Input::RegisterHandler("D", std::bind(&Character::Right, this));
		Engine::Input::RegisterHandler("\033[C", std::bind(&Character::Right, this));

		Engine::Input::RegisterHandler("a", std::bind(&Character::Left, this));
		Engine::Input::RegisterHandler("A", std::bind(&Character::Left, this));
		Engine::Input::RegisterHandler("\033[D", std::bind(&Character::Left, this));

		obj.OnTick = std::bind(&Character::OnTick, this);
		layer->AddObject(&obj);
	}
};

struct GravityBox
{
	Engine::Object obj;

	bool onGround = false;

	unsigned fallingSpeed;

	void OnTick()
	{
		// If catched, do not fall.
		for (size_t i = 0; i < fallingSpeed; i++)
			obj.Move({ 0, 1 });
	}

	GravityBox(Engine::Layer* layer, Engine::Vector2D pos, unsigned fallingSpeed = 1U) : fallingSpeed(fallingSpeed)
	{
		obj.pos = pos;

		obj.textures.resize(1);
		obj.textures[0].Write(
			{
				"#-#",
				"|G|",
				"#-#"
			}, { 100, 0, 200, 1.0f }, { 0, 0, 0, 0.1f }, { 0, 0 }
		);

		obj.colliders.push_back(Engine::Collider({ 3, 3 }, { 0, 0 }, 2));
		
		obj.OnTick = std::bind(&GravityBox::OnTick, this);
		
		layer->AddObject(&obj);
	}
};

struct AutoUpdatingText
{
	long* data;
	Engine::Object obj;
	long threshold;

	void OnTick()
	{
		if (*data > threshold)
			obj.textures[0].Write({std::to_string(*data)}, {255, 0, 0, 1.0f}, {0, 0, 0, 0.5f}, {0, 0});
		else
			obj.textures[0].Write({std::to_string(*data)}, {255, 255, 255, 1.0f}, {0, 0, 0, 0.5f}, {0, 0});
	}

	AutoUpdatingText(long* data, Engine::Vector2D pos, Engine::Layer* layer, long threshold) : data(data), threshold(threshold)
	{
		obj.textures.resize(1);
		obj.pos = pos;
		obj.OnTick = std::bind(&AutoUpdatingText::OnTick, this);
		layer->AddObject(&obj);
	}
};

bool charCamOn = false;

void TurnOnCharacterCamera() {
	charCamOn = !charCamOn;
}

int main()
{
	Engine::AudioSource backgroundMusic;
	// backgroundMusic.LoadWavFile("Assets/bordercrossing.wav");
	// backgroundMusic.Play(0, 0, 0, 0.8f);

	Engine::InitializeAudio();
	Engine::PrepareTerminal({50, 50});

	Engine::colliderTypes = {
		{ 0, 1, 1, 2 }, // Heavy - 0
		{ 0, 1, 1, 2 }, // Normal - 1
		{ 0, 0, 1, 2 }, // Light - 2
		{ 2, 2, 2, 2 } // Overlappable - 3
	};

	Engine::Map map;
	pmap = &map;

	Engine::Layer layer;
	map.AddLayer(&layer);

	Engine::Camera camera({ 0, 0 }, { 50, 50 });
	map.AddCamera(&camera, true);

	Engine::Object worldProps({ 1, 1 }, "");
	worldProps.textures.resize(3);
	worldProps.textures[0].File("Assets/sky.kcget", { 0, 0 });
	worldProps.textures[1].File("Assets/sky2.kcget", { 0, 30 });
	worldProps.textures[2].File("Assets/land.kcget", { 0, 20 });
	int base = 29;
	worldProps.colliders.push_back(Engine::Collider({ 1, 50 }, { 0, base }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 1, 50 }, { 1, base + 2 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 1, 50 }, { 2, base + 3 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 1, 50 }, { 3, base + 5 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 2, 50 }, { 4, base + 6 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 2, 50 }, { 6, base + 7 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 5, 50 }, { 8, base + 8 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 14, 50 }, { 13, base + 9 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 6, 50 }, { 27, base + 10 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 5, 50 }, { 33, base + 11 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 3, 50 }, { 38, base + 12 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 2, 50 }, { 41, base + 13 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 2, 50 }, { 43, base + 14 }, 0));
	worldProps.colliders.push_back(Engine::Collider({ 3, 50 }, { 45, base + 15 }, 0));
	layer.AddObject(&worldProps);

	Engine::Object border({0, 0}, "L");
	border.textures.resize(4);
	border.textures[0].Block({ 50, 1 }, Engine::SuperChar('-', { 0, 0, 0, 1.0f }, { 255, 255, 255, 1.0f }), { 0, 0 });
	border.textures[1].Block({ 50, 1 }, Engine::SuperChar('-', { 0, 0, 0, 1.0f }, { 255, 255, 255, 1.0f }), { 0, 49 });
	border.textures[2].Block({ 1, 50 }, Engine::SuperChar('|', { 0, 0, 0, 1.0f }, { 255, 255, 255, 1.0f }), { 0, 0 });
	border.textures[3].Block({ 1, 50 }, Engine::SuperChar('|', { 0, 0, 0, 1.0f }, { 255, 255, 255, 1.0f }), { 49, 0 });
	border.colliders.push_back(Engine::Collider({ 50, 1 }, { 0, 0 }, 0));
	border.colliders.push_back(Engine::Collider({ 1, 50 }, { 0, 0 }, 0));
	border.colliders.push_back(Engine::Collider({ 50, 1 }, { 0, 49 }, 0));
	border.colliders.push_back(Engine::Collider({ 1, 50 }, { 49, 0 }, 0));
	layer.AddObject(&border);

	// Character character(&layer);
	Character character(&layer);
	map.AddCamera(&character.cam);

	GravityBox gbA(&layer, {10, 20}, 1);
	GravityBox gbB(&layer, {15, 20}, 2);
	GravityBox gbC(&layer, {30, 20}, 3);

	Engine::Object house({ 20, 32 }, "");
	house.textures.resize(1);
	house.textures[0].File("Assets/house.kcget", { 0, 0 });
	layer.AddObject(&house);

	Engine::Input::RegisterHandler("m", TurnOnCharacterCamera);

	Engine::Layer darkLayer;
	darkLayer.brgba = { 0, 0, 0, 0.5f };
	darkLayer.frgba = { 0, 0, 0, 0.5f };

	AutoUpdatingText audioPerformance(&Engine::audioPerformance, {1, 1}, &layer, 172);

	std::thread t_inputLoop(Engine::Input::Loop);

	for (Engine::tps = 24; Engine::running; Engine::thisTickStartTP.SetToNow(), Engine::totalTicks++)
	{
		Engine::Input::Call();
		Engine::CallOnTicks(&map);

		if (map.activeCameraI != -1 && map.activeCameraI < map.cameras.size())
		{
			// continue;
			if (charCamOn) {
				map.cameras[0]->Render({ &layer, &darkLayer });
				map.cameras[0]->Draw({ 0, 0 }, 0, 0, 0, 0);
				map.cameras[1]->Render(map.layers);
				map.cameras[1]->Draw({ 18, 18 }, 0, 0, 0, 0);
			}
			else {
				map.cameras[0]->Render(map.layers);
				map.cameras[0]->Draw({ 0, 0 }, 0, 0, 0, 0);
			}
			Engine::Print();
		}

		// std::cout << "n="<< Engine::totalTicks << " | delta=" << Engine::deltaTime << " | fps=" << Engine::fps << " | pfps=" << Engine::potentialfps << std::endl;
		// std::cout << "x=" << Engine::terminalSize.ws_col << ", y=" << Engine::terminalSize.ws_row << std::endl;
	
		Engine::WaitUntilNextTick();
	}
	Engine::TerminateAudio();
	Engine::ResetTerminal();
	exit(0);
}
