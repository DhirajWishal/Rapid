// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <volk.h>

#include <optional>

namespace rapid
{
	/**
	 * Queue object.
	 * This object represents all the necessary queues by the application.
	 */
	class Queue final
	{
	public:
		/**
		 * Default constructor.
		 */
		Queue() = default;

		/**
		 * Explicit constructor.
		 *
		 * @param vPhysicalDevice The physical device object.
		 */
		explicit Queue(VkPhysicalDevice vPhysicalDevice);

		/**
		 * Check if the queue is complete.
		 * This will only occur if both the transfer and graphics queues are present.
		 *
		 * @return Boolean value stating if its complete or not.
		 */
		bool isComplete() const;

		/**
		 * Get the transfer queue.
		 *
		 * @return The transfer queue.
		 */
		VkQueue getTransferQueue() const { return m_GraphicsQueue; }

		/**
		 * Get the transfer queue.
		 *
		 * @return The transfer queue.
		 */
		VkQueue& getTransferQueue() { return m_GraphicsQueue; }

		/**
		 * Get the graphics queue.
		 *
		 * @return The graphics queue.
		 */
		VkQueue getGraphicsQueue() const { return m_GraphicsQueue; }

		/**
		 * Get the graphics queue.
		 *
		 * @return The graphics queue.
		 */
		VkQueue& getGraphicsQueue() { return m_GraphicsQueue; }

		/**
		 * Get the transfer queue family.
		 *
		 * @return The transfer queue family.
		 */
		std::optional<uint32_t> getTransferFamily() const { return m_TransferFamily; }

		/**
		 * Get the graphics queue family.
		 *
		 * @return The graphics queue family.
		 */
		std::optional<uint32_t> getGraphicsFamily() const { return m_GraphicsFamily; }

	private:
		std::optional<uint32_t> m_TransferFamily;
		std::optional<uint32_t> m_GraphicsFamily;

		VkQueue m_TransferQueue = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
	};
}