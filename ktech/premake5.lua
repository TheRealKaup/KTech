--[[
Create your own Premake workspace and include this file to add
the library as a statically linkable Premake project and globally
link portaudio in your workspace.
]]--

links { "portaudio" }

project "KTechLibrary"
	kind "StaticLib"
	language "C++"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	files { "**cpp", "**hpp" }

	filter "configurations:Debug"
		symbols "On"
		defines { "DEBUG" }