// Copyright (c) 2022 Dhiraj Wishal

#include "Queue.hpp"

#include <spdlog/spdlog.h>
#include <vector>

namespace rapid
{
	Queue::Queue(VkPhysicalDevice vPhysicalDevice)
	{
		// Get the queue family count.
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &queueFamilyCount, nullptr);

		// Validate if we have queue families.
		if (queueFamilyCount == 0)
		{
			spdlog::error("Failed to get the queue family property count!");
			return;
		}

		// Get the queue family properties.
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		// Iterate over those queue family properties and find the most suitable one.
		for (uint32_t index = 0; index < queueFamilyCount; index++)
		{
			const auto& family = queueFamilies[index];
			if (family.queueCount == 0)
				continue;

			// Check if the queue flag contains what we want. If so, we can assign the queue family and return from the function.
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				m_GraphicsFamily = index;

			if (family.queueFlags & VK_QUEUE_TRANSFER_BIT)
				m_TransferFamily = index;

			// If the queue is complete, we can break.
			if (isComplete())
				break;
		}
	}

	bool Queue::isComplete() const
	{
		return m_TransferFamily.has_value() && m_GraphicsFamily.has_value();
	}
}