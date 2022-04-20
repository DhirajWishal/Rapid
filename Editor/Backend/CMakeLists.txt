# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_library(
	Backend
	STATIC

	GraphicsEngine.cpp
	GraphicsEngine.hpp
	Utility.cpp
	Utility.hpp
)

# Set the include directory.
target_include_directories(
	Backend 
	PUBLIC 

	${RAPID_EDITOR_INCLUDE_DIR} 
	${VOLK_INCLUDE_DIR} 
	${VMA_INCLUDE_DIR}
	${IMGUI_INCLUDE_DIR}
)

# Add the target link libraries.
target_link_libraries(
	Backend

	Core 
	volk
	spdlog
	imgui
)

# Set the C++ standard as C++20.
set_property(TARGET Backend PROPERTY CXX_STANDARD 20)