// Copyright (c) 2022 Dhiraj Wishal

#include "Buffer.hpp"
#include "Utility.hpp"

#include <spdlog/spdlog.h>

namespace rapid
{
	Buffer::Buffer(GraphicsEngine& engine, uint64_t size, BufferType type)
		: m_Engine(engine), m_Size(size), m_Type(type)
	{
		// Validate the inputs.
		if (m_Size == 0)
		{
			spdlog::error("Invalid buffer size!");
			return;
		}

		VmaAllocationCreateFlags vmaFlags = 0;
		auto memoryUsage = VMA_MEMORY_USAGE_UNKNOWN;

		// Setup memory usage.
		switch (m_Type)
		{
		case BufferType::Vertex:
		case BufferType::Index:
			memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			break;

		case BufferType::ShallowVertex:
		case BufferType::ShallowIndex:
			memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			break;

		case BufferType::Uniform:
		case BufferType::Staging:
			memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
			vmaFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			break;

		default:
			spdlog::error("Invalid buffer type!");
			return;
		}

		// Create the buffer.
		VkBufferCreateInfo crateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = m_Size,
			.usage = static_cast<VkBufferUsageFlags>(m_Type),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
		};

		VmaAllocationCreateInfo vmaAllocationCreateInfo = {
			.flags = vmaFlags,
			.usage = memoryUsage,
		};

		utility::ValidateResult(vmaCreateBuffer(engine.getAllocator(), &crateInfo, &vmaAllocationCreateInfo, &m_Buffer, &m_Allocation, nullptr), "Failed to create the buffer!");
	}

	Buffer::~Buffer()
	{
		if (isActive())
			terminate();
	}

	void Buffer::terminate()
	{
		// If the buffer is still mapped, let's unmap it.
		if (m_IsMapped)
			unmapMemory();

		vmaDestroyBuffer(m_Engine.getAllocator(), m_Buffer, m_Allocation);
		m_IsTerminated = true;
	}

	std::byte* Buffer::mapMemory()
	{
		std::byte* pDataPointer = nullptr;
		utility::ValidateResult(vmaMapMemory(m_Engine.getAllocator(), m_Allocation, reinterpret_cast<void**>(&pDataPointer)), "Failed to map the buffer memory!");

		m_IsMapped = true;
		return pDataPointer;
	}

	void Buffer::unmapMemory()
	{
		// We only need to unmap if we have mapped the memory.
		if (m_IsMapped)
		{
			vmaUnmapMemory(m_Engine.getAllocator(), m_Allocation);
			m_IsMapped = false;
		}
	}

	void Buffer::copyFrom(const Buffer& buffer)
	{
		// Validate the incoming buffer size.
		if (buffer.size() > m_Size)
		{
			spdlog::error("The source buffer size is larger than what's available!");
			return;
		}

		// Setup copy info.
		VkBufferCopy bufferCopy = {
			.srcOffset = 0,
			.dstOffset = 0,
			.size = buffer.size()
		};

		// Copy the buffer.
		const auto vCommandBuffer = m_Engine.beginCommandBufferRecording();
		m_Engine.getDeviceTable().vkCmdCopyBuffer(vCommandBuffer, buffer.m_Buffer, m_Buffer, 1, &bufferCopy);
		m_Engine.executeRecordedCommands();
	}
}