project "test_basic_types"
kind "ConsoleApp"
language "C++"
cppdialect "C++20"
targetdir "%{wks.location}/bin"
objdir "%{wks.location}/obj/%{prj.name}"
targetname "%{prj.name}"

-- Header-only basic types: include repo root so #include "ktech/..." works.
includedirs { ".." }

files { "test_basic_types.cpp" }

filter "configurations:Debug"
symbols "On"