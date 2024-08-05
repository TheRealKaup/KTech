-- Create a workspace that its projects will be built in `build/`
workspace "KTech"
	configurations { "Debug", "Release" }
	location "build"

-- Include the KTech library
include "ktech"

-- Include all the examples in `examples/`, they are configured so object files will be in `build/obj/<project_name>` and binary files will be in `build/bin/` 
for i,dir in ipairs(os.matchfiles("examples/*/premake5.lua")) do
    include(dir)
end

include("documentation/tutorial/")