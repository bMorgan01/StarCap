cmake_minimum_required(VERSION 3.12)
project(SFML_Template)

set(CMAKE_CXX_STANDARD 14)

include_directories("D:/Program Files/mingw-w64/mingw64/x86_64-w64-mingw32/include")

add_executable(SFML_Template main.cpp Game.cpp Game.h GameSprite.cpp GameSprite.h Ship.cpp Ship.h System.cpp System.h Planet.cpp Planet.h Collision.cpp Collision.h Menu.cpp Menu.h)

target_link_directories(SFML_Template PUBLIC "D:/Program Files/mingw-w64/mingw64/x86_64-w64-mingw32/lib")

target_link_libraries(SFML_Template sfml-graphics sfml-system sfml-window sfml-audio)
