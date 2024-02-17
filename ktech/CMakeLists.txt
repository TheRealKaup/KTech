set(LIBRARY_HEADERS
	ktech.hpp
	widgets/button.hpp
	widgets/intfield.hpp
	widgets/stringfield.hpp
	widgets/switch.hpp
	widgets/widget.hpp
)

set(LIBRARY_SOURCES
	${LIBRARY_HEADERS}
	audio.cpp
	camera.cpp
	collider.cpp
	collision.cpp
	input.cpp
	layer.cpp
	map.cpp
	memory.cpp
	object.cpp
	output.cpp
	texture.cpp
	time.cpp
	timepoint.cpp
)

add_library(ktech STATIC ${LIBRARY_SOURCES})
