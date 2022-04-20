// Copyright (c) 2022 Dhiraj Wishal

#define VMA_IMPLEMENTATION
#include "GraphicsEngine.hpp"
#include "Utility.hpp"

namespace rapid
{
	GraphicsEngine::GraphicsEngine()
	{
		// FIrst of all, initialize volk. Without this we can't do anything else.
		utility::ValidateResult(volkInitialize(), "Failed to initialize volk!");

		// Initialize the instance and the rest.
		createInstance();
		selectPhysicalDevice();
		createLogicalDevice();
	}

	GraphicsEngine::~GraphicsEngine()
	{
		if (!m_IsTerminated)
			terminate();
	}

	void GraphicsEngine::terminate()
	{
		m_IsTerminated = true;
	}

	void GraphicsEngine::createInstance()
	{
		VkApplicationInfo applicationInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "Rapid",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = volkGetInstanceVersion()
		};
	}

	void GraphicsEngine::selectPhysicalDevice()
	{
	}

	void GraphicsEngine::createLogicalDevice()
	{
	}
}