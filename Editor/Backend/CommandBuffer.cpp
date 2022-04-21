// Copyright (c) 2022 Dhiraj Wishal

#include "CommandBuffer.hpp"

namespace rapid
{
	CommandBuffer::CommandBuffer(GraphicsEngine& engine, VkCommandBuffer vCommandBuffer)
		: m_Engine(engine), m_CommandBuffer(vCommandBuffer)
	{
	}
}