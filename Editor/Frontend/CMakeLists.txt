# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_library(
	Frontend
	STATIC

	Dummy.cpp
)

# Set the include directory.
target_include_directories(Frontend PUBLIC ${RAPID_EDITOR_INCLUDE_DIR})

# Set the C++ standard as C++20.
set_property(TARGET Frontend PROPERTY CXX_STANDARD 20)