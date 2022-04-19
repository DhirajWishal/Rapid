# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_executable(
	RapidEditor
)

# Set the include directory.
target_include_directories(RapidEditor PUBLIC ${RAPID_EDITOR_INCLUDE_DIR})

# Set the C++ standard as C++20.
set_property(TARGET RapidEditor PROPERTY CXX_STANDARD 20)