# Copyright (c) 2022 Dhiraj Wishal
# Main build script for Rapid

# Set the minimum requierd CMake version.
cmake_minimum_required(3.18.4)

# Set the basic project description.
project(
	Rapid
	VERSION 1.0.0 
	DESCRIPTION "Visual programming editor primarily built for game engines."
)

# Set the main include directory for the engine.
set(RAPID_EDITOR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Editor)

# Add the required subdirectories.
add_subdirectory(Editor/Application)
add_subdirectory(Editor/Backend)
add_subdirectory(Editor/Core)
add_subdirectory(Editor/Frontend)