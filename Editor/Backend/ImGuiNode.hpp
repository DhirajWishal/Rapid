// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "ProcessingNode.hpp"
#include "Image.hpp"

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
		 */
		explicit ImGuiNode(GraphicsEngine& engine);

		/**
		 * Destructor.
		 */
		~ImGuiNode();

		/**
		 * Terminate the node.
		 */
		void terminate() override;

		/**
		 * Bind the resources to the command buffer.
		 *
		 * @param vCommandBuffer The command buffer to bind to.
		 */
		void bind(VkCommandBuffer vCommandBuffer) override;

	private:
		std::unique_ptr<Image> m_FontImage = nullptr;
	};
}