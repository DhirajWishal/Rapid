// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "GraphicsEngine.hpp"

namespace rapid
{
	class Window;
	class GraphicsPipeline;
	class ShaderResource;
	class Buffer;

	/**
	 * Command buffer object.
	 * This object is a wrapper for the Vulkan command buffer handle and contains the required methods to perform
	 * the required tasks.
	 */
	class CommandBuffer final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which this object is bound to.
		 * @param vCommandBuffer The command buffer.
		 */
		explicit CommandBuffer(GraphicsEngine& engine, VkCommandBuffer vCommandBuffer);

		/**
		 * Begin buffer recording.
		 */
		void begin();

		/**
		 * Bind a window to the command buffer.
		 *
		 * @param window The window to bind.
		 * @param vClearColors The screen clear color values.
		 */
		void bindWindow(const Window& window, const std::vector<VkClearValue>& vClearColors) const;

		/**
		 * Unbind the currently bound window.
		 */
		void unbindWindow() const;

		/**
		 * Bind a graphics pipeline to the command buffer.
		 *
		 * @param pipeline The pipeline to bind.
		 */
		void bindPipeline(const GraphicsPipeline& pipeline) const;

		/**
		 * Bind a shader resource.
		 *
		 * @param pipeline The pipeline.
		 * @param resource The shader resource to bind.
		 */
		void bindShaderResource(const GraphicsPipeline& pipeline, const ShaderResource& resource) const;

		/**
		 * Bind a vertex buffer to the command buffer.
		 *
		 * @param vertexBuffer The vertex buffer to bind.
		 */
		void bindVertexBuffer(const Buffer& vertexBuffer) const;

		/**
		 * Bind a index buffer to the command buffer.
		 *
		 * @param indexBuffer The index buffer to bind.
		 * @param indexType The index type of the buffer. Default is VK_INDEX_TYPE_UINT32.
		 */
		void bindIndexBuffer(const Buffer& indexBuffer, VkIndexType indexType = VK_INDEX_TYPE_UINT32) const;

		/**
		 * Bind a viewport to the command buffer.
		 *
		 * @param viewport The viewport to bind.
		 */
		void bindViewport(const VkViewport viewport) const;

		/**
		 * Bind a scissor to the command buffer.
		 *
		 * @param scissor The scissor to bind.
		 */
		void bindScissor(const VkRect2D scissor) const;

		/**
		 * Bind push constants to the command buffer.
		 *
		 * @param pipeline The pipeline to bind.
		 * @param pDataStore The data store to bind.
		 * @param size The size of data.
		 * @param flags The shader flags to which the data is sent.
		 */
		void bindPushConstant(const GraphicsPipeline& pipeline, const void* pDataStore, uint64_t size, VkShaderStageFlags flags) const;

		/**
		 * Draw vertices to the command buffer.
		 *
		 * @param vertexCount The vertex count to draw.
		 */
		void drawVertices(const uint32_t vertexCount) const;

		/**
		 * Draw indices to the command buffer.
		 * This will access the vertexes using the index buffer.
		 *
		 * @param indexCount The index count.
		 * @param indexOffset The index offset.
		 * @param vertexOffset The vertex offset of the vertex buffer to begin.
		 */
		void drawIndices(const uint32_t indexCount, const uint32_t indexOffset, const uint32_t vertexOffset) const;

		/**
		 * End buffer recording.
		 */
		void end();

		/**
		 * Submit the commands to the GPU.
		 *
		 * @param vRenderFinishedSemaphore The semaphore to be signaled.
		 * @param vInFlightSemaphore The wait semaphore.
		 * @param shouldWait Whether or not to wait till the submission finishes. Default is false.
		 */
		void submit(VkSemaphore& vRenderFinishedSemaphore, VkSemaphore& vInFlightSemaphore, bool shouldWait = false);

		/**
		 * Get the buffer primitive.
		 *
		 * @return The buffer.
		 */
		VkCommandBuffer buffer() const { return m_CommandBuffer; }

	private:
		GraphicsEngine& m_Engine;
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

		//VkFence m_ReadyFence = VK_NULL_HANDLE;

		bool m_IsRecording = false;
	};
}