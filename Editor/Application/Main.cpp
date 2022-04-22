// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/ImGuiNode.hpp"
#include "Backend/Window.hpp"
#include "Backend/ShaderCode.hpp"
#include "Backend/GraphicsPipeline.hpp"

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

		auto pipeline = rapid::GraphicsPipeline(engine, window, "PipelineCache.bin", vertexShader, fragmentShader);
		auto& resource = pipeline.createShaderResource();
	}

	uint64_t counter = 100000;
	while (counter--)
	{
		window.pollEvents();
		window.submitFrame();
	}

	window.terminate();

	return 0;
}