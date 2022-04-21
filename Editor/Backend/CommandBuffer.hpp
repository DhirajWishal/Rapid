// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "GraphicsEngine.hpp"

namespace rapid
{
    /**
     * Command buffer object.
     * This object is a wrapper for the Vulkan command buffer handle and contains the required methods to perform
     * the required tasks.
     */
    class CommandBuffer final
    {
    public:
        /**
         * Explicit constructor.
         *
         * @param engine The engine to which this object is bound to.
         * @param vCommandBuffer The command buffer.
         */
        //explicit CommandBuffer(GraphicsEngine& engine, VkCommandBuffer vCommandBuffer);

    private:
        GraphicsEngine& m_Engine;
        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

        bool m_IsRecording = false;
    };
}