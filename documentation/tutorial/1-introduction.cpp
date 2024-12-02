/*
	KTech's Tutorial, the official KTech tutorial taught by examples.
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

/*
	Welcome to the official KTech tutorial!

	This is my attempt at teaching a subject while putting everything into practice in an interactive fashion. Hopefully, by the end of this tutorial you'll not only know every aspect of KTech, but also have experience with every aspect of KTech. The latter can be as important as the former.

	When you come across something you don't know (that isn't about to be explained), pause, learn it, and only then come back; don't frustrate yourself and waste your time. I assume you know how to use the terminal and program C++ code, and that you cloned KTech's git repository to work on.

	The tutorial is separated into chapters, each one is a C++ source file that usually contains a heavily-commented example of functional KTech code (this file, "1-introduction.cpp", is the very first chapter). You should read the tutorial linearly if you are new to KTech; literally read the individual files from top to bottom, and the chapters in an ascending order.



	Let's start by getting things up and running; building, and running.

	Within this directory there's an additional file named "premake5.lua" - it is the configuration file for the command line tool Premake that KTech uses to generate build files. This particular configuration file will generate the build files for the entire tutorial and all its chapters at once. I will not teach you how to use Premake as it already has a fine documentation at https://premake.github.io/docs/, though knowing how to script it is not needed right now; we just want to run the configuration file we already have.

	First, generate build files. Ensure you have Premake installed on your system and run the following command from the root directory of your cloned repository ("gmake2" for GNU make files):
		`$ premake5 gmake2`
	Premake will read the "premake5.lua" configuration file present in the repository's root, which will in turn read the additional configuration files in KTech's and the tutorial's directories. The result is a directory named "build/" containing the build files for everything.

	Secondly, build. To generate binary files, run `make` on the GNU makefiles that were generated in the "build/" directory by Premake, as so:
		`$ make -C build/`
	`make` will build KTech as a static library, then this file and the following files in the tutorial as individual console applications. The result are executable binaries in "build/bin/".

	Lastly, run the following command to run the binary that was built out of this source file:
		`$ ./build/bin/1-introduction`
	And now you are running the program written in the end of this source file. Each chapter of the tutorial should have its own binary file.

	This has built the entire code of the tutorial now, but you may modify code later on. To update the binary files you should simply rebuild like was done in the second command.



	As you can see below, this file literally contains C++ code. Continue reading linearly.
*/

// "ktech/ktech.hpp" is KTech's main header file that you would normally include in your game. It will give you access to everything except the optional widgets (UI elements; we'll cover them later).
#include "../../ktech/ktech.hpp"

// With KTech, you still define your game's entry function.
int main()
{
	// Create an engine instance; we'll dive deeper into this in the next chapter.
	KTech::Engine engine(KTech::UPoint(9, 9), 24);

	// Everything in KTech is declared within the `KTech` namespace. You can declare a using-directive on it if you find it more convenient.
	using namespace KTech;

	/*
		You can proceed to the next chapter.
	*/

	// Do a couple of other things that'll get explained later:
	Map map(engine);
	Camera camera(engine, map.m_id, Point(0, 0), engine.output.resolution);
	Layer layer(engine, map.m_id);
	Object object(engine, layer.m_id, Point(2, 2));

	camera.m_background.b = KTech::RGB(140, 200, 240);

	object.m_textures.resize(2);
	object.m_textures.resize(2);
	object.m_textures[0].Write(
		{
			" @@@ ",
			"@@@@@",
			"@@@@@",
		}, RGBA(0, 100, 0), RGBAColors::transparent, Point(0, 0)
	);
	object.m_textures[1].Simple(UPoint(1, 2), CellA('|', KTech::RGBA(80, 40, 15, 255), KTech::RGBAColors::transparent), KTech::Point(2, 3));

	while (engine.running)
	{
		engine.input.CallCallbacks();
		engine.time.CallInvocations();
		engine.memory.CallOnTicks();

		if (engine.output.ShouldRenderThisTick())
		{
			camera.Render();
			engine.output.Draw(camera.m_image, camera.m_res);
			engine.output.Print();
		}
		else if (engine.output.ShouldPrintThisTick())
			engine.output.Print();

		engine.time.WaitUntilNextTick();
	}
}