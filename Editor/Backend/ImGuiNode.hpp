// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "ProcessingNode.hpp"
#include "Image.hpp"
#include "GraphicsPipeline.hpp"

namespace rapid
{
	/**
	 * ImGui node object.
	 * This node acts as a single processing unit in the rendering pipeline, and contains everything needed by ImGui to render to the screen.
	 */
	class ImGuiNode final : public ProcessingNode
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine object.
		 * @param window The window which owns the node.
		 */
		explicit ImGuiNode(GraphicsEngine& engine, Window& window);

		/**
		 * Destructor.
		 */
		~ImGuiNode();

		/**
		 * Terminate the node.
		 */
		void terminate() override;

		/**
		 * Update ImGUI on the new iteration.
		 */
		void onPollEvents() override;

		/**
		 * Bind the resources to the command buffer.
		 *
		 * @param commandBuffer The command buffer to bind to.
		 * @param frameIndex The frame's index number.
		 */
		void bind(CommandBuffer commandBuffer, uint32_t frameIndex) override;

	private:
		/**
		 * Update the buffers.
		 * This will get the data from ImGui and update the vertex and index buffers.
		 */
		void updateBuffers();

	private:
		std::vector<ShaderResource*> m_ShaderResources = {};

		std::unique_ptr<Image> m_FontImage = nullptr;
		std::unique_ptr<GraphicsPipeline> m_Pipeline = nullptr;
		std::unique_ptr<Buffer> m_VertexBuffer = nullptr;
		std::unique_ptr<Buffer> m_IndexBuffer = nullptr;
	};
}