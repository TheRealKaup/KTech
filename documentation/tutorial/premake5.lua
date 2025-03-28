project "1-introduction"
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	links { "KTechLibrary" }

	files { "1-introduction.cpp"  }

	filter "configurations:Debug"
		symbols "On"

project "2-world"
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	links { "KTechLibrary" }

	files { "2-world.cpp"  }

	filter "configurations:Debug"
		symbols "On"

project "3-game_loop"
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	links { "KTechLibrary" }

	files { "3-game_loop.cpp"  }

	filter "configurations:Debug"
		symbols "On"

project "4-input"
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	links { "KTechLibrary" }

	files { "4-input.cpp"  }

	filter "configurations:Debug"
		symbols "On"