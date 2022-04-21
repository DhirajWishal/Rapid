// Copyright (c) 2022 Dhiraj Wishal

#include "Backend/Buffer.hpp"
#include "Backend/Image.hpp"

#ifdef main 
#undef main
#endif

int main()
{
	rapid::GraphicsEngine engine;
	auto buffer = rapid::Buffer(engine, 1024, rapid::BufferType::Staging);
	auto buffer2 = rapid::Buffer(engine, 1024, rapid::BufferType::Staging);
	buffer2.copyFrom(buffer);

	auto image = rapid::Image(engine, { 1280, 720, 1 }, VkFormat::VK_FORMAT_R8G8B8A8_SRGB);
	image.changeImageLayout(VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	auto imageCopy = image.toBuffer();
	image.fromBuffer(*imageCopy);
	
	buffer.terminate();
	buffer2.terminate();
	image.terminate();

	return 0;
}