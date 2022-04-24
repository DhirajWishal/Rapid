# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_executable(
	RapidEditor

	Main.cpp
	Source/Application.cpp
	Source/Application.hpp
)

# Set the include directory.
target_include_directories(
	RapidEditor 
	PUBLIC 
	
	${RAPID_EDITOR_INCLUDE_DIR}
)

# Add the target link libraries.
target_link_libraries(RapidEditor Frontend)

# Set the C++ standard as C++20.
set_property(TARGET RapidEditor PROPERTY CXX_STANDARD 20)

# Copy the assets to the build output's application directory.
file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR}/Editor/Application)
file(COPY Fonts DESTINATION ${CMAKE_BINARY_DIR}/Editor/Application)
file(COPY Themes DESTINATION ${CMAKE_BINARY_DIR}/Editor/Application)