project "SimplePlatform1"
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	links { "KTechLibrary" }

	files { "*.cpp", "*.hpp"  }