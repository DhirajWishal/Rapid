# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_library(
	Frontend
	STATIC
	
	Function.cpp
	Function.hpp
	Object.hpp 
	Defaults.hpp  
	Object.cpp 
	Variable.hpp
	Components/UIComponent.hpp
	Components/FileExplorer.cpp
	Components/FileExplorer.hpp
	Components/NodeEditor.cpp
	Components/NodeEditor.hpp
)

# Set the include directory.
target_include_directories(
	Frontend 
	PUBLIC 
	
	${RAPID_EDITOR_INCLUDE_DIR}
	${IMNODES_INCLUDE_DIR}
)

# Add the target link libraries.
target_link_libraries(Frontend Backend imnodes)

# Set the C++ standard as C++20.
set_property(TARGET Frontend PROPERTY CXX_STANDARD 20)