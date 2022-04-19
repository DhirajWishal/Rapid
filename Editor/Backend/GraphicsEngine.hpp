// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <volk.h>
#include <vk_mem_alloc.h>

namespace rapid
{
	/**
	 * Graphics engine object.
	 * This object contains the main graphics objects, namely the instance, logical and physical devices.
	 */
	class GraphicsEngine final
	{
	public:
		/**
		 * Constructor.
		 * Note that this will automatically initialize the object by allocating Vulkan objects.
		 */
		GraphicsEngine();

		/**
		 * Destructor.
		 */
		~GraphicsEngine();

		/**
		 * Terminate the engine.
		 */
		void terminate();

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

		bool m_IsTerminated = false;
	};
}