cmake_minimum_required(VERSION 3.26.4)

project(game)

add_executable(${PROJECT_NAME}
    game.cpp
    engine/audio.cpp
    engine/camera.cpp
    engine/engine.cpp
    engine/input.cpp
    engine/map.cpp
    engine/physics.cpp
    engine/structs.cpp
)

target_link_directories(game PUBLIC "/usr/lib")

target_link_libraries(game portaudio)