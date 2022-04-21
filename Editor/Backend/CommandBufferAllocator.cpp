// Copyright (c) 2022 Dhiraj Wishal

#include "CommandBufferAllocator.hpp"
#include "Utility.hpp"

namespace rapid
{
    CommandBufferAllocator::CommandBufferAllocator(GraphicsEngine &engine, uint8_t count)
        : m_Engine(engine), m_BufferCount(count)
    {
        // Create the command pool.
        VkCommandPoolCreateInfo commandPoolCreateInfo = {
          .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
          .pNext = nullptr,
          .flags = 0,
          .queueFamilyIndex = m_Engine.getQueue().getGraphicsFamily().value()
        };

        utility::ValidateResult(m_Engine.getDeviceTable().vkCreateCommandPool(m_Engine.getLogicalDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");

        // Allocate the command buffers.
        VkCommandBufferAllocateInfo allocateInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = m_CommandPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = m_BufferCount
        };

        std::vector<VkCommandBuffer> vCommandBuffers(m_BufferCount);
        utility::ValidateResult(m_Engine.getDeviceTable().vkAllocateCommandBuffers(m_Engine.getLogicalDevice(), &allocateInfo, vCommandBuffers.data()), "Failed to allocate command buffers!");
    }

    CommandBufferAllocator::~CommandBufferAllocator()
    {
        if(isActive())
            terminate();
    }

    void CommandBufferAllocator::terminate()
    {
        m_Engine.getDeviceTable().vkDestroyCommandPool(m_Engine.getLogicalDevice(), m_CommandPool, nullptr);
        m_IsTerminated = true;
    }
}