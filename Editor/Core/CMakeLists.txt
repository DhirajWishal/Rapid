# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_library(
	Core
	STATIC

	UndoStack.cpp
	UndoStack.hpp
	Limiter.cpp
	Limiter.hpp
)

# Set the include directory.
target_include_directories(
	Core 
	PUBLIC 
	
	${RAPID_EDITOR_INCLUDE_DIR}
)

# Set the C++ standard as C++20.
set_property(TARGET Core PROPERTY CXX_STANDARD 20)