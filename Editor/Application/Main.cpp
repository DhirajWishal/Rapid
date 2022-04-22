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

	uint64_t counter = 100000;
	while (counter--)
	{
		window.pollEvents();

		ImGui::Begin("Something");
		ImGui::Text("This is the first time I'm rendering text to check if everything is working fine!");
		ImGui::End();

		window.submitFrame();
	}

	window.terminate();

	return 0;
}