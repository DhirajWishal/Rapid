# Copyright (c) 2022 Dhiraj Wishal

# Add the test executable.
add_library(
	Frontend
	STATIC
	
	Components/Function.cpp
	Components/Function.hpp
	Components/Object.hpp 
	Components/Defaults.hpp  
	Components/Object.cpp 
	Components/Variable.hpp
	UIComponent.hpp
	FileExplorer.cpp
	FileExplorer.hpp
	NodeEditor.cpp
	NodeEditor.hpp
	CodeView.cpp
	CodeView.hpp
	MenuBar.cpp
	MenuBar.hpp
	NodeSpawner.cpp
	NodeSpawner.hpp
	Console.cpp
	Console.hpp
	Utility/ThemeParser.cpp
	Utility/ThemeParser.hpp
	Utility/CloseEvent.hpp
	Globals.cpp
	Globals.hpp
	CodeGenerator.cpp
	CodeGenerator.hpp
)

# Set the include directory.
target_include_directories(
	Frontend 
	PUBLIC 
	
	${RAPID_EDITOR_INCLUDE_DIR}
	${IMNODES_INCLUDE_DIR}
	${JSON_INCLUDE_DIR}
)

# Add the target link libraries.
target_link_libraries(Frontend Backend imnodes)

# Set the C++ standard as C++20.
set_property(TARGET Frontend PROPERTY CXX_STANDARD 20)