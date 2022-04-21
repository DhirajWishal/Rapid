// Copyright (c) 2022 Dhiraj Wishal

#include "Window.hpp"
#include "GraphicsEngine.hpp"

#include <spdlog/spdlog.h>
#include <SDL_vulkan.h>

namespace rapid
{
	Window::Window(GraphicsEngine& engine, std::string_view title)
		: m_Engine(engine), m_pWindow(SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE))
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
		if (m_Surface)
			vkDestroySurfaceKHR(m_Engine.getInstance(), m_Surface, nullptr);
		m_IsTerminated = true;
	}

	void Window::createSurface()
	{
		SDL_Vulkan_CreateSurface(m_pWindow, m_Engine.getInstance(), &m_Surface);
	}
}