// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/ImGuiNode.hpp"
#include "Backend/Window.hpp"
#include "Backend/ShaderCode.hpp"
#include "Backend/GraphicsPipeline.hpp"

#include "Frontend/Components/FileExplorer.hpp"

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

	while (window.pollEvents())
	{
		//ImGui::ShowDemoWindow();
		ImGui::Begin("Style editor");
		ImGui::ShowFontSelector("Fonts");
		ImGui::End();

		explorer.begin();
		explorer.end();

		window.submitFrame();
	}

	window.terminate();

	return 0;
}