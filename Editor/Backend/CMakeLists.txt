# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_library(
	Backend
	STATIC

	GraphicsEngine.cpp
	GraphicsEngine.hpp
	Utility.cpp
	Utility.hpp
	Window.cpp 
	Window.hpp
	Queue.cpp
	Queue.hpp
	Buffer.cpp
	Buffer.hpp
	BackendObject.hpp
	Image.cpp
	Image.hpp
	ImGuiNode.cpp
	ImGuiNode.hpp
	ProcessingNode.hpp
	CommandBufferAllocator.cpp
	CommandBufferAllocator.hpp
	CommandBuffer.cpp
	CommandBuffer.hpp
	ShaderCode.hpp
	ShaderCode.cpp
	GraphicsPipeline.cpp
	GraphicsPipeline.hpp
 "ShaderResource.hpp" "ShaderResource.cpp")

# Set the include directory.
target_include_directories(
	Backend 
	PUBLIC 

	${RAPID_EDITOR_INCLUDE_DIR} 
	${VOLK_INCLUDE_DIR} 
	${VMA_INCLUDE_DIR}
	${IMGUI_INCLUDE_DIR}
	${SDL_INCLUDE_DIR}
	${SPIRV_REFLECT_INCLUDE_DIR}
)

# Add the target link libraries.
target_link_libraries(
	Backend

	Core 
	volk
	spdlog
	imgui
	SDL2
	SPIRV_Reflect
)

# Set the C++ standard as C++20.
set_property(TARGET Backend PROPERTY CXX_STANDARD 20)

# Make sure to add the -fconcepts flag if we are not in MSVC and not in APPLE.
if(NOT MSVC AND NOT APPLE)
	target_compile_options(Backend PUBLIC "-fconcepts")
endif()