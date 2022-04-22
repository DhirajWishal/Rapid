// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "GraphicsEngine.hpp"

namespace rapid
{
	/**
	 * Buffer type enum.
	 */
	enum class BufferType : uint32_t
	{
		// Used to store vertex data. Note that in order to supply data to this type, we need a staging buffer.
		Vertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

		// Used to store index data. Note that in order to supply data to this type, we need a staging buffer.
		Index = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

		// Used to store vertex data. Note that unlike the other, this can directly receive data.
		ShallowVertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

		// Used to store index data. Note that unlike the other, this can directly receive data.
		ShallowIndex = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

		// Used to store uniform data.
		Uniform = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,

		// Used for data transferring purposes.
		Staging = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
	};

	/**
	 * Buffer object.
	 * This object is used to store information about a single buffer.
	 */
	class Buffer final : public BackendObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The graphics engine.
		 * @param size The size of the buffer.
		 * @param type The buffer type.
		 */
		explicit Buffer(GraphicsEngine& engine, uint64_t size, BufferType type);

		/**
		 * Destructor.
		 */
		~Buffer();

		/**
		 * Terminate the object.
		 */
		void terminate() override;

		/**
		 * Map the buffer memory to the local address space.
		 *
		 * @return The byte pointer.
		 */
		std::byte* mapMemory();

		/**
		 * Unmap the memory from the local address space.
		 */
		void unmapMemory();

		/**
		 * Copy content from another buffer to this.
		 *
		 * @param buffer The other buffer to copy from.
		 */
		void copyFrom(const Buffer& buffer);

		/**
		 * Get the size of the buffer.
		 *
		 * @return The size.
		 */
		uint64_t size() const { return m_Size; }

		/**
		 * Get the type of the buffer.
		 *
		 * @return The buffer type.
		 */
		BufferType type() const { return m_Type; }

		/**
		 * Get the Vulkan buffer handle.
		 *
		 * @return The buffer.
		 */
		VkBuffer buffer() const { return m_Buffer; }

	private:
		GraphicsEngine& m_Engine;

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = nullptr;

		const uint64_t m_Size;
		const BufferType m_Type;

		bool m_IsMapped = false;
	};
}