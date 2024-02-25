-- Create a workspace that its projects will be built in `build/`
workspace "KTech"
	configurations { "Release" }
	location "build"

-- Include the KTech library
include "ktech"

-- Build the examples, they are configured so object files will be in `build/obj/<project_name>` and binary files will be in `build/bin/` 
include "examples/simpleplatform1"
include "examples/widgetstest"