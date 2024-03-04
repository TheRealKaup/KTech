#include "../../ktech/ktech.hpp"

// Create engine
static KTech::Engine engine(KTech::UPoint(40, 20), 24);

struct Character : KTech::Object
{
	void Down()
	{
		Move(KTech::Point(0, 1));
	}
	void Up()
	{
		Move(KTech::Point(0, -1));
	}
	void Right()
	{
		Move(KTech::Point(1, 0));
	}
	void Left()
	{
		Move(KTech::Point(-1, 0));
	}

	Character(KTech::Engine& engine, KTech::ID<KTech::Layer>& layer, KTech::Point pos)
		: Object(engine, layer, pos, "character")
	{
		textures.resize(1);
		textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 200, 0, 0, 255 }, { 0, 0, 0, 0 }, { 0, 0 }
		);
		colliders.resize(1);
		colliders[0].ByTextureCharacter(textures[0], 100, 1);

		engine.io.RegisterCallback(KTech::Keys::down, std::bind(&Character::Down, this), true);
		engine.io.RegisterCallback(KTech::Keys::up, std::bind(&Character::Up, this), true);
		engine.io.RegisterCallback(KTech::Keys::right, std::bind(&Character::Right, this), true);
		engine.io.RegisterCallback(KTech::Keys::left, std::bind(&Character::Left, this), true);

		EnterLayer(layer);
	}
};

int main()
{
	// Create world
	KTech::Map map(engine);

	KTech::Camera camera(engine, KTech::Point(0, 0), KTech::UPoint(40, 20));
	camera.background = KTech::CellA(' ', KTech::RGBA(0, 0, 0, 0), KTech::RGBA(180, 230, 240, 255));
	map.AddCamera(camera.id);

	KTech::Layer layer(engine, map.id);

	KTech::Object tree(engine, KTech::Point(17, 7));
	tree.textures.resize(2); // Add textures to the object
	tree.textures[0].Write(
		{
			" @@@ ",
			"@@@@@",
			"@@@@@",
		}, KTech::RGBA(0, 100, 0), KTech::RGBAColors::transparent, KTech::Point(0, -1)
	);
	tree.textures[1].Simple(KTech::UPoint(1, 2), KTech::CellA('|', KTech::RGBA(80, 40, 15, 255), KTech::RGBAColors::transparent), KTech::Point(2, 2));
	tree.colliders.resize(2); // Add colliders to textures
	tree.colliders[0].ByTextureCharacter(tree.textures[0], 1, 0);
	tree.colliders[1].Simple(KTech::UPoint(1, 2), 0, KTech::Point(2, 2));
	tree.EnterLayer(layer.id);

	Character character(engine, layer.id, KTech::Point(2, 2));

	// Game loop
	while (engine.running)
	{
		// Start
		engine.time.StartThisTick();

		// Calls
		engine.io.Call();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		// Render, draw and print
		camera.Render(map.layers);
		engine.io.Draw(camera.image, KTech::Point(0, 0), 0, 0, 0, 0, engine.time.ticksCounter % 256);
		engine.io.Print();

		// End
		engine.time.WaitUntilNextTick();
	}
}