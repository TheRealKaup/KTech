set(EXECUTABLE_NAME widgetstest)

add_executable(${EXECUTABLE_NAME}
	game.cpp
)

target_link_directories(${EXECUTABLE_NAME} PUBLIC "/usr/lib")

target_link_libraries(${EXECUTABLE_NAME} ktech)
