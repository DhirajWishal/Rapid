// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Window.hpp"
#include "Queue.hpp"

#include <vk_mem_alloc.h>
#include <memory>

namespace rapid
{
	/**
	 * Graphics engine object.
	 * This object contains the main graphics objects, namely the instance, logical and physical devices.
	 */
	class GraphicsEngine final : public BackendObject
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
		void terminate() override;

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

		/**
		 * Begin utility command buffer recording.
		 *
		 * @return The command buffer.
		 */
		VkCommandBuffer beginCommandBufferRecording();

		/**
		 * End the utility command buffer recording.
		 */
		void endCommandBufferRecording();

		/**
		 * Execute the recorded commands.
		 *
		 * @param shouldWait Whether or not to wait until the command are executed. Default is true.
		 */
		void executeRecordedCommands(bool shouldWait = true);

		/**
		 * Get the device table.
		 *
		 * @return The device table.
		 */
		const VolkDeviceTable& getDeviceTable() const { return m_DeviceTable; }

		/**
		 * Get the physical device.
		 *
		 * @return The physical device.
		 */
		VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }

		/**
		 * Get the physical device properties.
		 *
		 * @return The physical device properties.
		 */
		const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() const { return m_Properties; }

		/**
		 * Get the VMA allocator.
		 *
		 * @return The allocator.
		 */
		VmaAllocator getAllocator() const { return m_vAllocator; }

		/**
		 * Get the logical device.
		 *
		 * @return The logical device.
		 */
		VkDevice getLogicalDevice() const { return m_LogicalDevice; }

		/**
		 * Get the instance.
		 *
		 * @return The instance.
		 */
		VkInstance getInstance() const { return m_Instance; }

		/**
		 * Get the queue object.
		 *
		 * @return The queue object.
		 */
		Queue getQueue() const { return m_Queue; }

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
		 * Get the VMA functions.
		 *
		 * @return The functions needed by VMA.
		 */
		VmaVulkanFunctions getVmaFunctions() const;

		/**
		 * Create the logical device.
		 */
		void createLogicalDevice();

		/**
		 * Setup ImGui.
		 */
		void setupImGui() const;

		/**
		 * Create the utility command buffer.
		 */
		void createUtilityCommandBuffer();

	private:
		VolkDeviceTable m_DeviceTable = {};
		VkPhysicalDeviceProperties m_Properties = {};

		Queue m_Queue = {};

		std::unique_ptr<Window> m_Window = nullptr;

		std::vector<const char*> m_ValidationLayers = {};
		std::vector<const char*> m_DeviceExtensions = {};

		VmaAllocator m_vAllocator;

		VkInstance m_Instance = VK_NULL_HANDLE;

#ifdef RAPID_DEBUG
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

#endif

		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

		bool m_IsRecording = false;
	};
}