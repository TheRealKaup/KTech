cmake_minimum_required(VERSION 3.26.4)

project(game)

add_executable(${PROJECT_NAME}
    game.cpp
    ../../ktech/audio.cpp
    ../../ktech/camera.cpp
    ../../ktech/collider.cpp
    ../../ktech/collision.cpp
    ../../ktech/engine.cpp
    ../../ktech/input.cpp
    ../../ktech/layer.cpp
    ../../ktech/map.cpp
    ../../ktech/object.cpp
    ../../ktech/texture.cpp
    ../../ktech/time.cpp
    ../../ktech/timepoint.cpp
)

target_link_directories(game PUBLIC "/usr/lib")

target_link_libraries(game portaudio)