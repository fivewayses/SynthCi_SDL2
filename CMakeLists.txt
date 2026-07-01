cmake_minimum_required(VERSION 3.12)

project(SynthC VERSION 1.0.0 LANGUAGES CXX)

add_executable(main
	main.cpp
)

find_package(SDL2 REQUIRED)

target_link_libraries(main SDL2::SDL2)


