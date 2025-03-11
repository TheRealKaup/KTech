/*
	quickstart, an introductory KTech game.
	Copyright (C) 2023-2025 Ethan Kaufman (AKA Kaup)

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

/*
	Reading this file is best done by starting from the `main()` function.
	At the end of reading this example you'll know how to create a moving character.
*/

// Include KTech
#include "../../ktech/ktech.hpp"

// Derived from the `Object` world structure. This way we can add whatever functionality we want to `Object`, and in this case, moving.
struct Character : KTech::Object
{
	// Create a `Camera`. The second parameter is the so-called "global" position and the third is the resolution.
	KTech::Camera camera;
	// Set `camera`'s background to a blue color so we have a sky.
	// `Camera::m_background` is of type `Cell`, meaning you can give the background a character (`Cell:c`) and a foreground color (`Cell::f`) as well. Here, though, we only set the background value (`Cell::b`).

	KTech::Input::CallbackGroup callbackGroup;

	void OnMove(KTech::Point dir) override
	{
		camera.m_pos = m_pos + KTech::Point(-18, -8);
	}

	Character(KTech::Engine& engine, const KTech::ID<KTech::Layer>& layer, const KTech::ID<KTech::Map>& map, KTech::Point pos)
		: Object(engine, layer, pos, "character"), // Inherit `Object::Object()`, since `Object` has no default constructor. The fourth name parameter can be useful for debugging if so interests you, though it bears no additional functionality.
		camera(engine, KTech::Point(pos.x - 18, pos.y - 8), KTech::UPoint(40, 20)),  callbackGroup(engine, true)
	{
		// Add a singular texture.
		m_textures.resize(1);
		m_textures[0].Write(
			{ // Stick figure.
				" O ",
				"/|\\",
				"/ \\"
			}, KTech::RGBA(200, 0, 0, 255) /*Red*/, KTech::RGBAColors::transparent /*No background*/, KTech::Point(0, 0) /*No additional relative position*/
		);
		// Add a singular collider.
		m_colliders.resize(1);
		// Make it match the stick figure texture.
		m_colliders[0].ByTextureCharacter(m_textures[0], 1 /*Can push and be pushed by default.*/);

		// Register input callback functions to make the character movable by pressing keys.
		callbackGroup.RegisterCallback( // Move down
			KTech::Keys::down, // The triggering key. The `KTech::Keys` namespace includes a variety of different key values (in the form of escape sequences).
			std::bind( // We'll have to `std::bind` the callback function because it's a non-static member.
				&Character::Move, // Pointer to the callback function, `Object::Move()`, which requests `Collision` to move this object.
				this, // Bind this object to the function.
				KTech::Point(0, 1) // `Move()` has one `Point` parameter which represents where to attempt moving relatively (direction), so bind +1 on the Y axis (which means down) to it.
			));
		callbackGroup.RegisterCallback(KTech::Keys::up, std::bind(&Character::Move, this, KTech::Point(0, -1))); // Move up.
		callbackGroup.RegisterCallback(KTech::Keys::right, std::bind(&Character::Move, this, KTech::Point(1, 0))); // Move right.
		callbackGroup.RegisterCallback(KTech::Keys::left, std::bind(&Character::Move, this, KTech::Point(-1, 0))); // Move left.

		camera.m_background.b = KTech::RGB(180, 230, 240);
		// Add `camera` to `map`. World structures communicate with each other using their `ID`s (the `m_id` member). The second parameter sets `camera` to be active one in `map`.
		camera.EnterMap(map);

		// Challenge: currently there is nothing for the character instance to be pushed by (which is allowed by collider type we set a moment ago). So, copy this class, change its keybindings and create an instance of it along with the original character (but pass it a different initial position). Now you have two characters controlled by different sets of keys; try making them push each other.

	}
};

int main()
{
	// First and foremost, create an `Engine` instance. The first parameter is the viewport resolution, the second is the ticks-per-second (TPS) limit.
	KTech::Engine engine(KTech::UPoint(40, 20), 24);

	// Create a `Map`. When constructed, a world structures must be provided with a reference to an `Engine` instance so it can add itself to `Engine::memory`.
	KTech::Map map(engine);

	// Create a `Layer`. The second parameter will add it to `map` right away.
	KTech::Layer layer(engine, map.m_id);
	/*
	Adding one world structure to another can be done when they are constructed or later, and from the world structure that is being added or added to it.
	Meaning, you could add `camera` and `layer` to `map` in any of the following ways:
	```
	// From the constructors (by providing the optional second parameter with the parent's `ID`):
		KTech::Camera camera(engine, map.m_id, true, KTech::Point(0, 0), KTech::UPoint(40, 20));
		KTech::Layer layer(engine, map.m_id);
	// Using "adder" functions:
		map.AddCamera(camera.m_id, true);
		map.AddLayer(layer.m_id);
	// Using "enterer" functions:
		camera.EnterMap(map.m_id, true);
		layer.EnterMap(map.m_id);
	```
	All methods result the same.
	*/

	// Create an object in the shape of a tree. The third parameter is the position.
	KTech::Object tree(engine, layer.m_id, KTech::Point(17, 7));
	// To add textures, we first need to resize the `Object::m_textures` vector, which stores the object's textures (as simple instances, not `ID`s; textures are not meant to be shared or moved around).
	tree.m_textures.resize(2);
	// For the first texture, which will be the canopy, use `Texture::Write()`.
	tree.m_textures[0].Write(
		{ // Vector of strings for the characters of the texture.
			" @@@ ",
			"@@@@@",
			"@@@@@",
		},
		KTech::RGBA(0, 100, 0), // Foreground color (green). The alpha channel is an optional parameter that defaults to 255 (completely opaque).
		KTech::RGBAColors::transparent, // Background color. The `KTech::RGBAColors` and `KTech::RGBColors` namespaces contain some predefined `RGBA` and `RGB` values, respectively. Transparent (alpha value of 0, i.e. `RGBA(0, 0, 0, 0)`) here means no background, just character.
		KTech::Point(0, 0) // Position, relative to the parent object.
	);
	// For the second texture, which will be the trunk, use `Texture::Simple()`.
	tree.m_textures[1].Simple(
		KTech::UPoint(1, 30), // Size.
		KTech::CellA( // Texture value.
			'|', // Character.
			KTech::RGBA(80, 40, 15, 255), // Foreground color (brown). This time we also specify the alpha channel, though we don't actually need to because 255 is the default value anyway.
			KTech::RGBAColors::transparent // Background. Once again, none of that.
		),
		KTech::Point(2, 3) // Relative position.
	);

	// Similar to textures, adding colliders involves resizing a vector, this time `Object::m_colliders`.
	tree.m_colliders.resize(2);
	// For the first collider, which will be the canopy, use `ByTextureCharacter()` on the canopy texture. This will make the collider match the texture.
	tree.m_colliders[0].ByTextureCharacter(
		tree.m_textures[0], // Reference to the canopy texture.
		0 // Collider type. According to the default `Collision::colliderTypes` values, 0 means a collider that can push but can't be pushed.
		// The relative position is copied from the given texture, so no need to specify it here again.
	);
	// For the second collider, which will be the trunk, use `Collider::Simple`.
	tree.m_colliders[1].Simple(
		KTech::UPoint(1, 30), // Size.
		0, // Collider type that can't be pushed. You can set this to a collider type like 1 or 2 which will by default alow the tree object to be pushed specifically from its trunk; try it.
		KTech::Point(2, 3) // Relative position.
	);
	// The tree was already added to the layer, so adding it again (as follows) will do nothing.
	tree.EnterLayer(layer.m_id);

	// The way textures and colliders are managed might seem unconventional, but it's made this way to simplify changing them later while the game is running.

	// Create a character. You should now read the contents of the `Character` class.
	Character character(engine, layer.m_id, map.m_id, KTech::Point(2, 2));

	// Enter the game loop after initialization is done.
	while (engine.running)
	{
		// Call...
		engine.input.CallCallbacks(); // ...synchronized input callbacks (as were registered by `character`).
		engine.time.CallInvocations(); // ...timed invocations (though none were made here).
		engine.memory.CallOnTicks(); // ...on-tick functions (also left unutilized in this example).

		// Render-on-demand (when things have changed).
		if (engine.output.ShouldRenderThisTick()) // `Output::ShouldRenderThisTick()` reports whether the game changed.
		{
			character.camera.Render();
			/*
			Alternatively, you could write:
			- `camera.Render();`, which will renders the layers of its parent map.
			- `camera.Render(map.m_layers);`, which will render the given vector of layers. You can provide this function with any layer, no matter from what map or if in a map.
			*/
			// Draw the rendered image to `Output`'s "final" or "composed" image. Since `Camera::m_image` is a 1D array, the second parameter asks for its resolution.
			// You can provide `Output::Draw` with optional parameters to choose where to draw the image on the viewport and how to crop it.
			engine.output.Draw(character.camera.m_image, character.camera.m_res);
			// Print the final image.
			engine.output.Print();
		}
		else if (engine.output.ShouldPrintThisTick()) // `Output::ShouldPrintThisTick()` reports whether the terminal changed (which might require reprinting)
			engine.output.Print(); // Reprint the final image. It is already drawn; that hasn't changed.

		// Cause the thread to sleep until the moment the next tick should start.
		engine.time.WaitUntilNextTick();
	}
}