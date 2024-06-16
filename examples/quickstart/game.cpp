#include "../../ktech/ktech.hpp"

/*
I recommend reading `main()` before `Character`.
*/

struct Character : KTech::Object
{
	// The `Character` class will be derived from the `Object` class.

	Character(KTech::Engine& engine, KTech::ID<KTech::Layer>& layer, KTech::Point pos)
		: Object(engine, layer, pos, "character") // Inherit `Object::Object()`.
	{
		// Add one character `Texture`.
		m_textures.resize(1);
		m_textures[0].Write(
			{
				" O ",
				"/|\\",
				"/ \\"
			}, { 200, 0, 0, 255 }, { 0, 0, 0, 0 }, { 0, 0 }
		);
		m_colliders.resize(1);
		// Use the `Texture` to create a `Collider`.
		m_colliders[0].ByTextureCharacter(m_textures[0], 1 /*Can be pushed. There isn't anything to push it, though.*/);
		
		// Register input callbacks which will move our `Character`.
		engine.input.RegisterCallback( // Move down.
			KTech::Keys::down, // Key. The `KTech::Keys` namespace includes all  
			std::bind( // The callback function.
				&Character::Move, // `Object::Move()` asks `Collision` to move the `Object`.
				this, // Bind the function to this `Object`.
				KTech::Point(0, 1) // Where to move (this parameter goes to `Object::Move()`).
			),
			true // Call the callback function in `Input::CallHandlers()` (meaning, on-tick), or immediately when input is received (meaning, unsynced with game loop). The latter is preferable for UI, while on-tick is preferable for physics.
		);
		engine.input.RegisterCallback(KTech::Keys::up, std::bind(&Character::Move, this, KTech::Point(0, -1)), true); // Move up.
		engine.input.RegisterCallback(KTech::Keys::right, std::bind(&Character::Move, this, KTech::Point(1, 0)), true); // Move right.
		engine.input.RegisterCallback(KTech::Keys::left, std::bind(&Character::Move, this, KTech::Point(-1, 0)), true); // Move left.
	}
};

int main()
{
	// Create an engine instance. The first parameter is the viewport resolution, the second is the ticks per second limit.
	KTech::Engine engine(KTech::UPoint(40, 20), 24);
	
	// Create a map. You must provide world structures an `Engine` instance when constructed, so they can add themselves to `Engine.memory`.
	KTech::Map map(engine);
	// Create a camera. The first parameter is the position, the second is the resolution.
	KTech::Camera camera(engine, KTech::Point(0, 0), KTech::UPoint(40, 20));
	// Set the image background to blue.
	camera.m_background = KTech::Cell(' ', KTech::RGB(0, 0, 0), KTech::RGB(180, 230, 240));
	// Add the camera to the map. World structures communicate with each other using their `ID`s. The second parameter sets the camera as the map's active one.
	map.AddCamera(camera.m_id, true);
	// Create a layer. The second parameter will add it to the map right away.
	KTech::Layer layer(engine, map.m_id);

	/*
	Adding world structures to others can be done from their constructors or later.
	Meaning, you could have added the `camera` and `layer` we created to `map` in any of the following ways:
	```
	// From the constructors:
	KTech::Camera camera(engine, map.m_id, true, KTech::Point(0, 0), KTech::UPoint(40, 20));
	KTech::Layer layer(engine, map.m_id);
	// Using "adder" functions:
	map.AddCamera(camera.m_id, true);
	map.AddLayer(layer.m_id);
	// Using "enterer" functions:
	camera.EnterMap(map.m_id, true);
	layer.EnterMap(map.m_id);
	```
	*/

	// Create an `Object` in the shape of a tree. The second parameter is the position. 
	KTech::Object tree(engine, KTech::Point(17, 7));
	// To add `Texture`s, we first need to resize the `Object::m_textures` vector.
	tree.m_textures.resize(2);
	// For the first `Texture`, which will be the canopy, use `Texture::Write()`.
	tree.m_textures[0].Write(
		{ // Vector of strings for the characters of the texture.
			" @@@ ",
			"@@@@@",
			"@@@@@",
		},
		KTech::RGBA(0, 100, 0), // Foreground color (green).
		KTech::RGBAColors::transparent, // Background color. The `KTech::RGBAColors` and `KTech::RGBColors` namespaces contain some predefined `RGBA` adn `RGB` values.
		KTech::Point(0, 0) // Position, relative to the parent `Object`.
	);
	// For the second `Texture`, which will be the trunk, use `Texture::Simple()`.
	tree.m_textures[1].Simple(
		KTech::UPoint(1, 2), // Size.
		KTech::CellA( // Texture value.
			'|', // Character
			KTech::RGBA(80, 40, 15, 255), // Foreground.
			KTech::RGBAColors::transparent // Background.
		),
		KTech::Point(2, 3) // Position, relative to the parent `Object`.
	);
	// Similar to `Texture`s, to add `Collider`s, we first need to resize the `Object::m_scolliders` vector.
	tree.m_colliders.resize(2);
	// For the first collider, which will be the canopy, use `ByTextureCharacter()` on the canopy `Texture`. This will make the `Collider` correspond to the `Texture`. 
	tree.m_colliders[0].ByTextureCharacter(
		tree.m_textures[0], // Reference to the canopy `Texture`.
		0 // Collider type. The default `Collision::colliderTypes` values mean that it can't be pushed.
	);
	// For the second collider, which will be the trunk, use `Collider::Simple`. 
	tree.m_colliders[1].Simple(
		KTech::UPoint(1, 2), // Size.
		0, // Collider type.
		KTech::Point(2, 3) // Position, relative to the parent `Object`. 
	);
	// Add `tree` to `layer`.
	tree.EnterLayer(layer.m_id);

	// Create a `Character` instance.
	Character character(engine, layer.m_id, KTech::Point(2, 2));

	// Enter the game loop after initialization is done.
	while (engine.running)
	{
		// Call...
		engine.input.CallHandlers(); // ...input callbacks
		engine.time.CallInvocations(); // ...timed invocations
		engine.memory.CallOnTicks(); // ...on-tick functions

		// Render-on-demand, that is, render only when things have changed.
		if (engine.output.ShouldRenderThisTick()) // `Output::ShouldRenderThisTick()` can tell you whether things have changed, so you should render.
		{
			// `map` will render using the active `Camera`.
			map.Render();
			/*
			Alternatively, you could have written:
			- `camera.Render();`, or,
			- `camera.Render(map.m_layers);`
			*/
			// Draw the rendered image to `Output`. Since `Camera::m_image` is a 1D array, you also have to provide the image's resolution.
			engine.output.Draw(camera.m_image, camera.m_res);
			// Print the final image.
			engine.output.Print();
			/*
			The `Draw` "middleman" function allows displaying multiple images on the screen.
			To do so, use the first 3 optional parameters of `Draw()`, for example:
			```
			// Trim 2 cells of the outline
			engine.output.Draw(
				camera.m_image,
				camera.m_res,
				KTech::Point(2, 2), // From where to start drawing, on the final image
				KTech::UPoint(0, 0), // From where to start drawing, on the given image 
				KTech::UPoint(camera.m_res - KTech::UPoint(2, 2)) // From where to stop drawing, on the given image 
			);
			```
			*/
		}
		if (engine.output.ShouldPrintThisTick()) // Sometimes you don't need to render again, but still print due to the terminal changing in size.
			engine.output.Print(); // Print the already drawn image.

		// Enter sleep until the next tick.
		engine.time.WaitUntilNextTick();
	}
}