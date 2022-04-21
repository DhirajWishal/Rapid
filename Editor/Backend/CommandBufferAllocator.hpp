// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "GraphicsEngine.hpp"

namespace rapid
{
	/**
	 * Command buffer allocator object.
	 * This object is used to allocate command buffers and to manage them.
	 */
	class CommandBufferAllocator final : public BackendObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The graphics engine.
		 * @param count The command buffer count.
		 */
		//explicit CommandBufferAllocator(GraphicsEngine& engine, uint8_t count);

	private:
		GraphicsEngine& m_Engine;
		const uint8_t m_BufferCount;
	};
}