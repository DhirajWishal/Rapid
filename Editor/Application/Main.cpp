// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/ImGuiNode.hpp"
#include "Backend/Window.hpp"

#ifdef main 
#undef main
#endif

int main()
{
	rapid::GraphicsEngine engine;
	auto window = rapid::Window(engine, "Rapid Editor");
	window.createNode<rapid::ImGuiNode>();

	uint64_t counter = 1000000000;
	while (counter--)
	{
		window.pollEvents();
		window.submitFrame();
	}

	window.terminate();

	return 0;
}