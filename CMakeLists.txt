cmake_minimum_required(VERSION 3.20)
project(Tetris)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/F/L/SFML-2.5.1/lib/cmake/SFML/")

find_package(SFML REQUIRED system window graphics network audio)

link_directories("./win64-dynamic/lib")
include_directories("./win64-dynamic/include")

add_executable(Tetris src/main.cpp src/Tile.cpp src/Tile.h src/Global.h src/Core.h src/DiscordStatus.cpp src/DiscordStatus.h)

target_link_libraries(Tetris sfml-graphics sfml-audio opengl32 discord-rpc)
