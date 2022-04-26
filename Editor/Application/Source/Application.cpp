// Copyright (c) 2022 Dhiraj Wishal

#include "Application.hpp"

#include "Backend/ImGuiNode.hpp"
#include "Frontend/Console.hpp"

#include <imgui.h>
#include <fstream>

Application::Application()
	: m_Engine(rapid::GraphicsEngine())
	, m_Window(rapid::Window(m_Engine, "Rapid Editor"))
	, m_NodeEditor({})
	, m_Limiter(60)
{
	// Create the node.
	m_Window.createNode<rapid::ImGuiNode>();

	showSourceCode();

	while (m_Window.pollEvents())
	{
		ImGui::ShowDemoWindow();
		//m_Limiter.tick();

		// Show the menu bar.
		singleShot(m_MenuBar);

		// Show the file explorer in a single shot.
		singleShot(m_FileExplorer);

		// Show the code view.
		singleShot(m_CodeView);

		// Show the node editor.
		singleShot(m_NodeEditor);

		// Show the console.
		singleShot(rapid::GetConsole());

		// Finally submit the frame.
		m_Window.submitFrame();
	}

	// Make sure to terminate the window when exiting.
	m_Window.terminate();
}

void Application::singleShot(rapid::UIComponent& component) const
{
	component.begin();
	component.end();
}

void Application::showSourceCode()
{
	std::fstream sourceFile(__FILE__, std::ios::in | std::ios::ate);

	// Load the data if open.
	if (sourceFile.is_open())
	{
		const auto size = sourceFile.tellg();
		sourceFile.seekg(0);

		std::string source;
		source.resize(size);

		sourceFile.read(source.data(), size);
		m_CodeView.setCode(source);
	}

	sourceFile.close();
}
