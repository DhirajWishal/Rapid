// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "CommandBuffer.hpp"

namespace rapid
{
	/**
	 * Command buffer allocator object.
	 * This object is used to allocate command buffers and to manage them.
	 */
	class CommandBufferAllocator final : public BackendObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The graphics engine.
		 * @param count The command buffer count.
		 */
		explicit CommandBufferAllocator(GraphicsEngine& engine, uint8_t count);

		/**
		 * Destructor.
		 */
		~CommandBufferAllocator();

		/**
		 * Terminate the allocator.
		 */
		void terminate() override;

		/**
		 * Get the command buffers.
		 *
		 * @return The command buffers.
		 */
		std::vector<CommandBuffer>& getCommandBuffers() { return m_CommandBuffers; }

		/**
		 * Get the command buffers.
		 *
		 * @return The command buffers.
		 */
		const std::vector<CommandBuffer>& getCommandBuffers() const { return m_CommandBuffers; }

		/**
		 * Get a command buffer from the allocator.
		 *
		 * @param index The command buffer index.
		 * @return The command buffer.
		 */
		CommandBuffer getCommandBuffer(uint32_t index) const { return m_CommandBuffers[index]; }

	private:
		std::vector<CommandBuffer> m_CommandBuffers;

		GraphicsEngine& m_Engine;
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;

		const uint8_t m_BufferCount;
	};
}