// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "ProcessingNode.hpp"
#include "Image.hpp"
#include "GraphicsPipeline.hpp"

#include <chrono>

namespace rapid
{
	/**
	 * ImGui node object.
	 * This node acts as a single processing unit in the rendering pipeline, and contains everything needed by ImGui to render to the screen.
	 */
	class ImGuiNode final : public ProcessingNode
	{
		using clock_type = std::chrono::high_resolution_clock;
		using time_point = clock_type::time_point;

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
		 * 
		 * @param events The events.
		 */
		void onPollEvents(SDL_Event& events) override;

		/**
		 * Bind the resources to the command buffer.
		 *
		 * @param commandBuffer The command buffer to bind to.
		 * @param frameIndex The frame's index number.
		 */
		void bind(CommandBuffer commandBuffer, uint32_t frameIndex) override;

		/**
		 * This method will get called when the window is resized.
		 */
		void onWindowResize() override;

	private:
		/**
		 * Update the buffers.
		 * This will get the data from ImGui and update the vertex and index buffers.
		 */
		void updateBuffers();

		/**
		 * Resolve the keyboard inputs.
		 * 
		 * @param scancode The scancode.
		 * @param state The state.
		 */
		void resolveKeyboardInputs(SDL_Scancode scancode, bool state) const;

	private:
		time_point m_TimePoint;

		std::vector<ShaderResource*> m_ShaderResources = {};

		std::unique_ptr<Image> m_FontImage = nullptr;
		std::unique_ptr<GraphicsPipeline> m_Pipeline = nullptr;
		std::unique_ptr<Buffer> m_VertexBuffer = nullptr;
		std::unique_ptr<Buffer> m_IndexBuffer = nullptr;
	};
}