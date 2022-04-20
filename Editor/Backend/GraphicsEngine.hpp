// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Window.hpp"

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

		/**
		 * Begin frame function.
		 * This function is required to be called as the first function before calling anything else.
		 */
		void beginFrame();

		/**
		 * End frame function.
		 * This function must be called at the end to update all the internal components and render the scene.
		 */
		void endFrame();

	private:
		/**
		 * Initialize the instance.
		 */
		void createInstance();

		/**
		 * Select a suitable physical device.
		 */
		void selectPhysicalDevice();

		/**
		 * Create the logical device.
		 */
		void createLogicalDevice();

		/**
		 * Setup ImGui.
		 */
		void setupImGui() const;

	private:
		VolkDeviceTable m_DeviceTable = {};

		std::vector<Window> m_Windows = {};
		std::vector<const char*> m_ValidationLayers = {};

		VkInstance m_Instance = VK_NULL_HANDLE;

#ifdef RAPID_DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

#endif

		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

		bool m_IsTerminated = false;
	};
}