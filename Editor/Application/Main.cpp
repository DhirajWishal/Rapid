// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/ImGuiNode.hpp"
#include "Backend/Window.hpp"
#include "Backend/ShaderCode.hpp"
#include "Backend/GraphicsPipeline.hpp"

#include <imgui.h>

#ifdef main 
#undef main
#endif

int main()
{
	rapid::GraphicsEngine engine;
	auto window = rapid::Window(engine, "Rapid Editor");
	window.createNode<rapid::ImGuiNode>();

	while (window.pollEvents())
	{
		ImGui::ShowDemoWindow();
		window.submitFrame();
	}

	window.terminate();

	return 0;
}