// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/Buffer.hpp"

#ifdef main 
#undef main
#endif

int main()
{
	rapid::GraphicsEngine engine;
	auto buffer = rapid::Buffer(engine, 1024, rapid::BufferType::Staging);
	auto buffer2 = rapid::Buffer(engine, 1024, rapid::BufferType::Staging);
	buffer2.copyFrom(buffer);

	return 0;
}