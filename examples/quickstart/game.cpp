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
		m_textures.resize(1);
		m_textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 200, 0, 0, 255 }, { 0, 0, 0, 0 }, { 0, 0 }
		);
		m_colliders.resize(1);
		m_colliders[0].ByTextureCharacter(m_textures[0], 100, 1);

		engine.input.RegisterCallback(KTech::Keys::down, std::bind(&Character::Down, this), true);
		engine.input.RegisterCallback(KTech::Keys::up, std::bind(&Character::Up, this), true);
		engine.input.RegisterCallback(KTech::Keys::right, std::bind(&Character::Right, this), true);
		engine.input.RegisterCallback(KTech::Keys::left, std::bind(&Character::Left, this), true);

		EnterLayer(layer);
	}
};

int main()
{
	// Create world
	KTech::Map map(engine);

	KTech::Camera camera(engine, KTech::Point(0, 0), KTech::UPoint(40, 20));
	camera.m_background = KTech::CellA(' ', KTech::RGBA(0, 0, 0, 0), KTech::RGBA(180, 230, 240, 255));
	map.AddCamera(camera.m_id);

	KTech::Layer layer(engine, map.m_id);

	KTech::Object tree(engine, KTech::Point(17, 7));
	tree.m_textures.resize(2); // Add m_textures to the object
	tree.m_textures[0].Write(
		{
			" @@@ ",
			"@@@@@",
			"@@@@@",
		}, KTech::RGBA(0, 100, 0), KTech::RGBAColors::transparent, KTech::Point(0, -1)
	);
	tree.m_textures[1].Simple(KTech::UPoint(1, 2), KTech::CellA('|', KTech::RGBA(80, 40, 15, 255), KTech::RGBAColors::transparent), KTech::Point(2, 2));
	tree.m_colliders.resize(2); // Add m_colliders to m_textures
	tree.m_colliders[0].ByTextureCharacter(tree.m_textures[0], 1, 0);
	tree.m_colliders[1].Simple(KTech::UPoint(1, 2), 0, KTech::Point(2, 2));
	tree.EnterLayer(layer.m_id);

	Character character(engine, layer.m_id, KTech::Point(2, 2));

	// Game loop
	while (engine.running)
	{
		// Calls
		engine.input.CallHandlers();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		// Render, draw and print
		camera.Render(map.m_layers);
		engine.output.Draw(camera.m_image, KTech::Point(0, 0), 0, 0, 0, 0, engine.time.ticksCounter % 256);
		engine.output.Print();

		engine.time.WaitUntilNextTick();
	}
}