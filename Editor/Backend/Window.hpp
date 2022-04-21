// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "BackendObject.hpp"

#include <volk.h>
#include <SDL.h>
#include <string>
#include <vector>

namespace rapid
{
	class GraphicsEngine;

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
		 * Get the window extent.
		 *
		 * @return The extent.
		 */
		VkExtent2D extent() const;

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

	private:
		std::vector<VkImage> m_SwapchainImages = {};
		std::vector<VkImageView> m_SwapchainImageViews = {};

		GraphicsEngine& m_Engine;

		SDL_Window* m_pWindow = nullptr;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;

		VkFormat m_SwapchainFormat = VK_FORMAT_UNDEFINED;
	};
}