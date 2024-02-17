set(LIBRARY_NAME ktech)

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

add_library(${LIBRARY_NAME} STATIC ${LIBRARY_SOURCES})
target_link_libraries(${LIBRARY_NAME} portaudio)
target_include_directories(${LIBRARY_NAME} INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}
)
