// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Core/UndoStack.hpp"

#include "Backend/Window.hpp"

#include "Frontend/Components/FileExplorer.hpp"
#include "Frontend/Components/NodeEditor.hpp"
#include "Frontend/Components/MenuBar.hpp"
#include "Frontend/Components/CodeView.hpp"

/**
 * Application class.
 * This object is the main application and contains all the necessary objects and data required.
 */
class Application final
{
public:
	/**
	 * Default constructor.
	 */
	Application();

private:
	/**
	 * Show a UI component in a single shot.
	 *
	 * @param component The component to show.
	 */
	void singleShot(rapid::UIComponent& component) const;

	/**
	 * Show the current source code to the code view.
	 */
	void showSourceCode();

private:
	rapid::GraphicsEngine m_Engine;
	rapid::Window m_Window;

	rapid::UndoStack m_UndoStack;

	rapid::FileExplorer m_FileExplorer;
	rapid::NodeEditor m_NodeEditor;
	rapid::MenuBar m_MenuBar;
	rapid::CodeView m_CodeView;

	int32_t m_NodeAttributeID = 0;
};