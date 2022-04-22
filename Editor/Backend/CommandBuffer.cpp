// Copyright (c) 2022 Dhiraj Wishal

#include "CommandBuffer.hpp"
#include "Utility.hpp"
#include "Window.hpp"
#include "GraphicsPipeline.hpp"
#include "Buffer.hpp"

#include <spdlog/spdlog.h>

namespace rapid
{
	CommandBuffer::CommandBuffer(GraphicsEngine& engine, VkCommandBuffer vCommandBuffer)
		: m_Engine(engine), m_CommandBuffer(vCommandBuffer)
	{
	}

	void CommandBuffer::begin()
	{
		// If we are already recording, lets end it.
		if (m_IsRecording)
			end();

		// Begin the buffer.
		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pInheritanceInfo = nullptr
		};

		utility::ValidateResult(m_Engine.getDeviceTable().vkBeginCommandBuffer(m_CommandBuffer, &beginInfo), "Failed to begin command buffer recording!");
		m_IsRecording = true;
	}

	void CommandBuffer::bindWindow(const Window& window, const std::vector<VkClearValue>& vClearColors) const
	{
		VkRenderPassBeginInfo renderPassBeginInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext = VK_NULL_HANDLE,
			.renderPass = window.getRenderPass(),
			.framebuffer = window.getCurrentFrameBuffer(),
			.renderArea = {
				.extent = window.extent()
			},
			.clearValueCount = static_cast<uint32_t>(vClearColors.size()),
			.pClearValues = vClearColors.data(),
		};

		m_Engine.getDeviceTable().vkCmdBeginRenderPass(m_CommandBuffer, &renderPassBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
	}

	void CommandBuffer::unbindWindow() const
	{
		m_Engine.getDeviceTable().vkCmdEndRenderPass(m_CommandBuffer);
	}

	void CommandBuffer::bindPipeline(const GraphicsPipeline& pipeline) const
	{
		m_Engine.getDeviceTable().vkCmdBindPipeline(m_CommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline());
	}

	void CommandBuffer::bindPipeline(const GraphicsPipeline& pipeline, const ShaderResource& resource) const
	{
		const auto vDescriptorSet = resource.getDescriptorSet();
		m_Engine.getDeviceTable().vkCmdBindDescriptorSets(m_CommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, &vDescriptorSet, 0, nullptr);
		m_Engine.getDeviceTable().vkCmdBindPipeline(m_CommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline());
	}

	void CommandBuffer::bindVertexBuffer(const Buffer& vertexBuffer) const
	{
		// Validate the buffer type.
		if (vertexBuffer.type() != BufferType::Vertex)
		{
			spdlog::error("Cannot bind the buffer as a Vertex buffer! The types does not match.");
			return;
		}

		// Now we can bind it.
		constexpr std::array<VkDeviceSize, 1> offset = { 0 };
		const auto vBuffer = vertexBuffer.buffer();

		m_Engine.getDeviceTable().vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &vBuffer, offset.data());
	}

	void CommandBuffer::bindIndexBuffer(const Buffer& indexBuffer, VkIndexType indexType) const
	{
		// Validate the buffer type.
		if (indexBuffer.type() != BufferType::Index)
		{
			spdlog::error("Cannot bind the buffer as a Index buffer! The types does not match.");
			return;
		}

		// Now we can bind it.
		m_Engine.getDeviceTable().vkCmdBindIndexBuffer(m_CommandBuffer, indexBuffer.buffer(), 0, indexType);
	}

	void CommandBuffer::bindViewport(const VkViewport viewport) const
	{
		m_Engine.getDeviceTable().vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
	}

	void CommandBuffer::bindScissor(const VkRect2D scissor) const
	{
		m_Engine.getDeviceTable().vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
	}

	void CommandBuffer::drawVertices(const uint32_t vertexCount) const
	{
		m_Engine.getDeviceTable().vkCmdDraw(m_CommandBuffer, vertexCount, 1, 0, 0);
	}

	void CommandBuffer::drawIndices(const uint32_t indexCount, const uint32_t vertexOffset) const
	{
		m_Engine.getDeviceTable().vkCmdDrawIndexed(m_CommandBuffer, indexCount, 1, 0, vertexOffset, 0);
	}

	void CommandBuffer::end()
	{
		// Just return if we are not recording.
		if (!m_IsRecording)
			return;

		utility::ValidateResult(m_Engine.getDeviceTable().vkEndCommandBuffer(m_CommandBuffer), "Failed to end command buffer recording!");
		m_IsRecording = false;
	}
}