// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/ImGuiNode.hpp"
#include "Backend/Window.hpp"
#include "Backend/ShaderCode.hpp"

#ifdef main 
#undef main
#endif

int main()
{
	rapid::GraphicsEngine engine;
	auto window = rapid::Window(engine, "Rapid Editor");
	window.createNode<rapid::ImGuiNode>();

	{
		const auto vertexShader = rapid::ShaderCode("Shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
		const auto fragmentShader = rapid::ShaderCode("Shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	uint64_t counter = 1000000000;
	while (counter--)
	{
		window.pollEvents();
		window.submitFrame();
	}

	window.terminate();

	return 0;
}