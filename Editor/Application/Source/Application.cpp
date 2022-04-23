// Copyright (c) 2022 Dhiraj Wishal

#include "Application.hpp"

#include "Backend/ImGuiNode.hpp"

#include <imgui.h>
#include <fstream>

Application::Application()
	: m_Engine(rapid::GraphicsEngine())
	, m_Window(rapid::Window(m_Engine, "Rapid Editor"))
	, m_NodeEditor({})
{
	// Create the node.
	m_Window.createNode<rapid::ImGuiNode>();

	// Create the nodes.
	auto& nodeA = m_NodeEditor.createNode("Node A");
	nodeA.addInputAttribute("something", m_NodeAttributeID++);
	nodeA.addOutputAttribute("anything", m_NodeAttributeID++);

	auto& nodeB = m_NodeEditor.createNode("Node B");
	nodeB.addInputAttribute("another", m_NodeAttributeID++);
	nodeB.addOutputAttribute("stinky", m_NodeAttributeID++);

	auto& nodeC = m_NodeEditor.createNode("Node C");
	nodeC.addInputAttribute("frontend", m_NodeAttributeID++);
	nodeC.addOutputAttribute("network", m_NodeAttributeID++);
	nodeC.addOutputAttribute("backend", m_NodeAttributeID++);

	showSourceCode();

	while (m_Window.pollEvents())
	{
		ImGui::ShowDemoWindow();

		// Show the menu bar.
		singleShot(m_MenuBar);

		// Show the file explorer in a single shot.
		singleShot(m_FileExplorer);

		// Show the code view.
		singleShot(m_CodeView);

		// Show the node editor.
		singleShot(m_NodeEditor);

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
