// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/ImGuiNode.hpp"

#ifdef main 
#undef main
#endif

int main()
{
	rapid::GraphicsEngine engine;
	auto node = rapid::ImGuiNode(engine);
	node.terminate();

	return 0;
}