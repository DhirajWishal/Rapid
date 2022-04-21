# Copyright (c) 2022 Dhiraj Wishal
# Main build script for Rapid

# Set the minimum required CMake version.
cmake_minimum_required(VERSION 3.18.4)

# Set the basic project description.
project(
	Rapid
	VERSION 1.0.0
	DESCRIPTION "Visual programming editor primarily built for game engines."
)

# Set the main include directory for the engine.
set(RAPID_EDITOR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Editor)

# Set the Vulkan headers include directory.
set(VULKAN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Vulkan-Headers/include)

# Set the volk include directory and the library project
set(VOLK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/volk)

add_library(
	volk
	STATIC

	${VOLK_INCLUDE_DIR}/volk.c
)

set_property(TARGET volk PROPERTY CXX_STANDARD 20)

if(NOT MSVC)
	set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
endif()

# Add the Vulkan include directory for the volk library.
target_include_directories(volk PUBLIC ${VULKAN_INCLUDE_DIR})

# Set the Vulkan memory allocator include directory.
set(VMA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/VulkanMemoryAllocator/include)

# Set the SPIRV Reflect include directory and the library project.
set(SPIRV_REFLECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SPIRV-Reflect)

add_library(
	SPIRV_Reflect
	STATIC

	${SPIRV_REFLECT_INCLUDE_DIR}/spirv_reflect.cpp
)

set_property(TARGET SPIRV_Reflect PROPERTY CXX_STANDARD 20)

# Set the spdlog library and add it as a target library.
set(SPDLOG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/include)

add_library(
	spdlog
	STATIC

	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/src/async.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/src/cfg.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/src/color_sinks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/src/file_sinks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/src/fmt.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/src/spdlog.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/src/stdout_sinks.cpp
)

set_property(TARGET spdlog PROPERTY CXX_STANDARD 20)

# Set the imgui library and add it as a target library.
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui)

add_library(
	imgui
	STATIC

	${IMGUI_INCLUDE_DIR}/imgui.cpp
	${IMGUI_INCLUDE_DIR}/imgui_demo.cpp
	${IMGUI_INCLUDE_DIR}/imgui_draw.cpp
	${IMGUI_INCLUDE_DIR}/imgui_tables.cpp
	${IMGUI_INCLUDE_DIR}/imgui_widgets.cpp
)

set_property(TARGET imgui PROPERTY CXX_STANDARD 20)
target_include_directories(spdlog PUBLIC ${SPDLOG_INCLUDE_DIR})
target_compile_definitions(spdlog PUBLIC SPDLOG_COMPILED_LIB)

# Add the sdl library as a subdirectory and set the include directory.
add_subdirectory(ThirdParty/SDL)
set(SDL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/SDL/include)

# Set global compile definitions.
add_compile_definitions(
	$<$<CONFIG:Debug>:RAPID_DEBUG>
	$<$<CONFIG:Release>:RAPID_RELEASE>

	$<$<CONFIG:RelWithDebInfo>:RAPID_DEBUG>
	$<$<CONFIG:MinSizeRel>:RAPID_RELEASE>
	
	$<$<PLATFORM_ID:Windows>:RAPID_PLATFORM_WINDOWS>
	$<$<PLATFORM_ID:Linux>:RAPID_PLATFORM_LINUX>
	$<$<PLATFORM_ID:Darwin>:RAPID_PLATFORM_MAC>
)

# Add the required subdirectories.
add_subdirectory(Editor/Application)
add_subdirectory(Editor/Backend)
add_subdirectory(Editor/Core)
add_subdirectory(Editor/Frontend)