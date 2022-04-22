// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "CommandBufferAllocator.hpp"
#include "ProcessingNode.hpp"

namespace rapid
{
	/**
	 * Window class.
	 * This contains the basic information about the window, and all the rendering parts are done here.
	 */
	class Window final : public BackendObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine reference.
		 * @param title The window title.
		 */
		explicit Window(GraphicsEngine& engine, std::string_view title);

		/**
		 * Destructor.
		 */
		~Window();

		/**
		 * Terminate the object.
		 */
		void terminate() override;

		/**
		 * Poll the events.
		 * This needs to be called as the first function in every iteration.
		 */
		void pollEvents();

		/**
		 * Submit the frame to the GPU.
		 */
		void submitFrame();

		/**
		 * Create a new node.
		 *
		 * @tparam Type The node type.
		 * @tparam Args The constructor argument types.
		 * @param arguments The constructor arguments apart from GraphicsEngine and Window.
		 * @return The created object reference.
		 */
		template<node_type Type, class...Args>
		Type& createNode(Args&&... arguments) { return static_cast<Type&>(*m_ProcessingNodes.emplace_back(std::make_unique<Type>(m_Engine, *this, std::forward<Args>(arguments)...))); }

		/**
		 * Get the window extent.
		 *
		 * @return The extent.
		 */
		VkExtent2D extent() const;

		/**
		 * Get the render pass.
		 *
		 * @return The render pass.
		 */
		VkRenderPass getRenderPass() const { return m_RenderPass; }

		/**
		 * Get the current frame buffer.
		 *
		 * @return The frame buffer.
		 */
		VkFramebuffer getCurrentFrameBuffer() const { return m_Framebuffers[m_FrameIndex]; }

	private:
		/**
		 * Get the best buffer count.
		 *
		 * @return The buffer count.
		 */
		uint32_t getBestBufferCount() const;

		/**
		 * Clear the swapchain.
		 */
		void clearSwapchain();

		/**
		 * Resolve the swapchain image views.
		 */
		void resolveImageViews();

		/**
		 * Create the swapchain.
		 */
		void createSwapchain();

		/**
		 * Create the required render pass.
		 */
		void createRenderPass();

		/**
		 * Create the frame buffers.
		 */
		void createFramebuffers();

	private:
		std::vector<VkImage> m_SwapchainImages = {};
		std::vector<VkImageView> m_SwapchainImageViews = {};
		std::vector<VkFramebuffer> m_Framebuffers = {};
		std::vector<std::unique_ptr<ProcessingNode>> m_ProcessingNodes = {};

		std::unique_ptr<CommandBufferAllocator> m_CommandBufferAllocator = nullptr;

		GraphicsEngine& m_Engine;

		SDL_Window* m_pWindow = nullptr;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		VkFormat m_SwapchainFormat = VK_FORMAT_UNDEFINED;

		uint32_t m_FrameCount = 0;
		uint32_t m_FrameIndex = 0;
	};
}