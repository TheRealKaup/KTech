--[[
Create your own Premake workspace and include this file to add
the library as a statically linkable Premake project.
]]--

project "KTechLibrary"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir "%{wks.location}/bin"
	objdir "%{wks.location}/obj/%{prj.name}"
	targetname "%{prj.name}"

	files { "**cpp", "**hpp" }

	filter "configurations:Debug"
		symbols "On"
		defines { "DEBUG" }