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
	}

	void GraphicsEngine::selectPhysicalDevice()
	{
	}

	void GraphicsEngine::createLogicalDevice()
	{
	}
}