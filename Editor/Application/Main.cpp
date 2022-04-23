// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/ImGuiNode.hpp"
#include "Backend/Window.hpp"
#include "Backend/ShaderCode.hpp"
#include "Backend/GraphicsPipeline.hpp"

#include "Frontend/Components/FileExplorer.hpp"
#include "Frontend/Components/NodeEditor.hpp"

#include <imgui.h>

#ifdef main 
#	undef main

#endif

int main()
{
	rapid::GraphicsEngine engine;
	auto window = rapid::Window(engine, "Rapid Editor");
	window.createNode<rapid::ImGuiNode>();

	auto explorer = rapid::FileExplorer();
	auto nodeEditor = rapid::NodeEditor();

	auto nodeA = rapid::NodeBuilder("Node A", 0);
	nodeA.addInputAttribute("something", 0);
	nodeA.addOutputAttribute("anything", 1);

	auto nodeB = rapid::NodeBuilder("Node B", 1);
	nodeB.addInputAttribute("another", 2);
	nodeB.addOutputAttribute("stinky", 3);

	auto nodeC = rapid::NodeBuilder("Node C", 2);
	nodeC.addInputAttribute("frontend", 4);
	nodeC.addOutputAttribute("network", 5);
	nodeC.addOutputAttribute("backend", 6);

	while (window.pollEvents())
	{
		//ImGui::ShowDemoWindow();
		ImGui::Begin("Style editor");
		ImGui::ShowFontSelector("Fonts");
		ImGui::End();

		explorer.begin();
		explorer.end();

		nodeEditor.begin();
		nodeA.show();
		nodeB.show();
		nodeC.show();
		nodeEditor.end();

		window.submitFrame();
	}

	window.terminate();

	return 0;
}