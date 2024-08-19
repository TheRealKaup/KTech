project "1-introduction.cpp"
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	links { "KTechLibrary" }
	
	files { "1-introduction.cpp"  }

	filter "configurations:Debug"
		symbols "On"

project "2-world.cpp"
	kind "ConsoleApp"
	language "C++"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	links { "KTechLibrary" }
	
	files { "2-world.cpp"  }

	filter "configurations:Debug"
		symbols "On"