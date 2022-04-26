// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Core/UndoStack.hpp"
#include "Core/Limiter.hpp"

#include "Backend/Window.hpp"

#include "Frontend/FileExplorer.hpp"
#include "Frontend/NodeEditor.hpp"
#include "Frontend/MenuBar.hpp"
#include "Frontend/CodeView.hpp"

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
	rapid::Limiter m_Limiter;

	rapid::FileExplorer m_FileExplorer;
	rapid::NodeEditor m_NodeEditor;
	rapid::MenuBar m_MenuBar;
	rapid::CodeView m_CodeView;
};