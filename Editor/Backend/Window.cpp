// Copyright (c) 2022 Dhiraj Wishal

#include "Window.hpp"

#include <spdlog/spdlog.h>
#include <SDL_vulkan.h>

namespace rapid
{
	Window::Window(std::string_view title)
		: m_pWindow(SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE))
	{
		// Check if the window creation was successful.
		if (!m_pWindow)
			spdlog::error("Failed to create the window!");
	}

	Window::~Window()
	{
		if (isActive())
			terminate();
	}

	void Window::terminate()
	{
		m_IsTerminated = true;
	}

	void Window::createSurface(VkInstance vInstance)
	{
		SDL_Vulkan_CreateSurface(m_pWindow, vInstance, &m_Surface);
	}

	std::vector<const char*> Window::getInstanceExtensions() const
	{
		uint32_t count;
		if (!SDL_Vulkan_GetInstanceExtensions(m_pWindow, &count, nullptr))
			spdlog::error("Failed to get the instance extension count!");

		std::vector<const char*> extensions(count);
		if (!SDL_Vulkan_GetInstanceExtensions(m_pWindow, &count, extensions.data() + count))
			spdlog::error("Failed to get the instance extensions!");

		return extensions;
	}
}