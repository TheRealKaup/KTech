#include "../../engine/engine.hpp"

bool catchingCharacterB = false;

Engine::Map* pmap;

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

		if (!priorOnGround && onGround) groundHitSFX.Play(0, 0, 0, 0.5f);
	}

	Character(Engine::Layer* layer)
	{
		groundHitSFX.LoadWavFile("assets/groundHit.wav");
		jumpSFX.LoadWavFile("assets/jump.wav");

		obj.pos = { 5, 2 };//
		obj.textures.resize(1);
		obj.textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 255, 255, 0, 255 }, { 0, 0, 0, 0 }, { 0, 0 }
		);
		obj.colliders.resize(3);
		obj.colliders[0].Simple(Engine::UPoint(1, 2), 1, Engine::Point(0, 1));
		obj.colliders[1].Simple(Engine::UPoint(1, 2), 1, Engine::Point(1, 0));
		obj.colliders[2].Simple(Engine::UPoint(1, 2), 1, Engine::Point(2, 1));

		cam = Engine::Camera(Engine::Point( 0, 0 ), Engine::UPoint( 15, 15 ));

		Engine::Input::RegisterHandler("w", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterHandler("W", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterHandler(" ", std::bind(&Character::Jump, this), true);
		Engine::Input::RegisterHandler(Engine::Input::K::up, std::bind(&Character::Jump, this), true);

		Engine::Input::RegisterHandler("d", std::bind(&Character::Right, this));
		Engine::Input::RegisterHandler("D", std::bind(&Character::Right, this));
		Engine::Input::RegisterHandler(Engine::Input::K::right, std::bind(&Character::Right, this));

		Engine::Input::RegisterHandler("a", std::bind(&Character::Left, this));
		Engine::Input::RegisterHandler("A", std::bind(&Character::Left, this));
		Engine::Input::RegisterHandler(Engine::Input::K::left, std::bind(&Character::Left, this));

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

		obj.OnTick = std::bind(&GravityBox::OnTick, this);
		
		layer->AddObject(&obj);
	}
};

struct AutoUpdatingText
{
	float* data;
	Engine::Object obj;

	void OnTick()
	{
		obj.textures[0].Write({std::to_string(*data)}, {255, 255, 255, 255}, {0, 0, 0, 127}, {0, 0});
	}

	AutoUpdatingText(float* data, Engine::Point pos, Engine::Layer* layer) : data(data)
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
	using namespace Engine;

	colliderTypes = { // For "simpleplatform1" example
		{ CR::B, CR::P, CR::P, CR::O }, // Heavy - 0
		{ CR::B, CR::P, CR::P, CR::O }, // Normal - 1
		{ CR::B, CR::B, CR::P, CR::O }, // Light - 2
		{ CR::O, CR::O, CR::O, CR::O } // Overlappable - 3
	};

	Engine::InitializeAudio();
	Engine::PrepareTerminal({50, 50});

	Engine::Map map;
	pmap = &map;

	Engine::Layer layer;
	map.AddLayer(&layer);

	Engine::Camera camera({ 0, 0 }, { 50, 50 });
	map.AddCamera(&camera, true);

	Engine::Object worldProps({ 1, 1 }, "");
	worldProps.textures.resize(3);
	Engine::Log("<Main> Loading assets/sky.ktecht", RGBColors::blue);
	worldProps.textures[0].File("assets/sky.ktecht", { 0, 0 });
	Engine::Log("<Main> Loading assets/sky2.ktecht", RGBColors::blue);
	worldProps.textures[1].File("assets/sky2.ktecht", { 0, 30 }), Engine::CellA(' ');
	Engine::Log("<Main> Loading assets/land.ktecht", RGBColors::blue);
	worldProps.textures[2].File("assets/land.ktecht", { 0, 20 });
	
	worldProps.colliders.resize(14);
	uint8_t base = 29;
	worldProps.colliders[0].Simple(Engine::UPoint(1, 1), 0, Engine::Point(0, base));
	worldProps.colliders[1].Simple(Engine::UPoint(1, 1), 0, Engine::Point(1, base + 2));
	worldProps.colliders[2].Simple(Engine::UPoint(1, 1), 0, Engine::Point(2, base + 3));
	worldProps.colliders[3].Simple(Engine::UPoint(1, 1), 0, Engine::Point(3, base + 5));
	worldProps.colliders[4].Simple(Engine::UPoint(2, 1), 0, Engine::Point(4, base + 6));
	worldProps.colliders[5].Simple(Engine::UPoint(2, 1), 0, Engine::Point(6, base + 7));
	worldProps.colliders[6].Simple(Engine::UPoint(5, 1), 0, Engine::Point(8, base + 8));
	worldProps.colliders[7].Simple(Engine::UPoint(14, 1), 0, Engine::Point(13, base + 9));
	worldProps.colliders[8].Simple(Engine::UPoint(6, 1), 0, Engine::Point(27, base + 10));
	worldProps.colliders[9].Simple(Engine::UPoint(5, 1), 0, Engine::Point(33, base + 11));
	worldProps.colliders[10].Simple(Engine::UPoint(3, 1), 0, Engine::Point(38, base + 12));
	worldProps.colliders[11].Simple(Engine::UPoint(2, 1), 0, Engine::Point(41, base + 13));
	worldProps.colliders[12].Simple(Engine::UPoint(2, 1), 0, Engine::Point( 43, base + 14));
	worldProps.colliders[13].Simple(Engine::UPoint(3, 1), 0, Engine::Point(45, base + 15));
	layer.AddObject(&worldProps);

	Engine::Object frame(Engine::Point(0, 0), "L");
	frame.textures.resize(4);
	frame.textures[0].Simple({ 50, 1 }, Engine::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), { 0, 0 });
	frame.textures[1].Simple({ 50, 1 }, Engine::CellA('-', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), { 0, 49 });
	frame.textures[2].Simple({ 1, 50 }, Engine::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), { 0, 0 });
	frame.textures[3].Simple({ 1, 50 }, Engine::CellA('|', { 0, 0, 0, 255 }, { 255, 255, 255, 255 }), { 49, 0 });
	frame.colliders.resize(4);
	frame.colliders[0].Simple(Engine::UPoint(50, 1), 0, Engine::Point(0, 0));
	frame.colliders[1].Simple(Engine::UPoint(1, 50), 0, Engine::Point(0, 0));
	frame.colliders[2].Simple(Engine::UPoint(50, 1), 0, Engine::Point(0, 49));
	frame.colliders[3].Simple(Engine::UPoint(1, 50), 0, Engine::Point(49, 0));
	layer.AddObject(&frame);

	// Character character(&layer);
	Character character(&layer);
	map.AddCamera(&character.cam);

	GravityBox gbA(&layer, {10, 20}, 1);
	GravityBox gbB(&layer, {15, 20}, 2);
	GravityBox gbC(&layer, {30, 20}, 3);

	Engine::Object house({ 20, 32 }, "");

	house.textures.resize(1);
	Engine::Log("<Main> Loading house", RGBColors::blue);
	house.textures[0].File("assets/house.ktecht", { 0, 0 });
	layer.AddObject(&house);

	Engine::Input::RegisterHandler("m", TurnOnCharacterCamera);

	Engine::Layer darkLayer;
	darkLayer.alpha = 127;
	
	AutoUpdatingText audioPerformance(&Engine::Time::potentialfps, {1, 1}, &layer);

	std::thread t_inputLoop(Engine::Input::Loop);

	Engine::Time::tps = 24;

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
		Engine::Time::WaitUntilNextTick();
	}
	
	Engine::TerminateAudio();
	Engine::ResetTerminal();
	exit(0);
}