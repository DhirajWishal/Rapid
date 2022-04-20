// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <volk.h>
#include <SDL.h>
#include <string>
#include <vector>

namespace rapid
{
	/**
	 * Window class.
	 * This contains the basic information about the window, and all the rendering parts are done here.
	 */
	class Window final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param title The window title.
		 */
		explicit Window(std::string_view title);

		/**
		 * Create the Vulkan surface.
		 *
		 * @param vInstance The Vulkan instance.
		 */
		void createSurface(VkInstance vInstance);

		/**
		 * Get the required instance extensions.
		 *
		 * @return The extension names vector.
		 */
		std::vector<const char*> getInstanceExtensions() const;

	private:
		SDL_Window* m_pWindow = nullptr;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	};
}