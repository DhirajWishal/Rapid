# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_library(
	Backend
	STATIC
)

# Set the include directory.
target_include_directories(Backend PUBLIC ${RAPID_EDITOR_INCLUDE_DIR})

# Set the C++ standard as C++20.
set_property(TARGET Backend PROPERTY CXX_STANDARD 20)