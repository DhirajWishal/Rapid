// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Buffer.hpp"

#include <memory>

namespace rapid
{
	/**
	 * Image object.
	 * This object stores a single 2D image.
	 */
	class Image final : public BackendObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The graphics engine.
		 */
		explicit Image(GraphicsEngine& engine, VkExtent3D extent, VkFormat format);

		/**
		 * Destructor.
		 */
		~Image();

		/**
		 * Terminate the image object.
		 */
		void terminate() override;

		/**
		 * Change the image layout of the image.
		 *
		 * @param newLayout The new layout to set.
		 * @param vCommandBuffer The command buffer to use. Default is VK_NULL_HANDLE.
		 */
		void changeImageLayout(const VkImageLayout newLayout, const VkCommandBuffer vCommandBuffer = VK_NULL_HANDLE);

		/**
		 * Copy data from a stagging buffer.
		 *
		 * @param pBuffer The buffer to copy data from.
		 */
		void fromBuffer(const Buffer& buffer);

		/**
		 * Copy the whole image to a buffer.
		 *
		 * @return The copied buffer.
		 */
		std::unique_ptr<Buffer> toBuffer();

		/**
		 * Get the image extent.
		 *
		 * @return The image extent.
		 */
		VkExtent3D extent() const { return m_Extent; }

		/**
		 * Get the image format.
		 *
		 * @return The format.
		 */
		VkFormat format() const { return m_Format; }

		/**
		 * Get the Vulkan image.
		 *
		 * @return The image.
		 */
		VkImage image() const { return m_Image; }

		/**
		 * Get the image view.
		 *
		 * @return The image view.
		 */
		VkImageView view() const { return m_ImageView; }

		/**
		 * Get the image sampler.
		 *
		 * @return The sampler.
		 */
		VkSampler sampler() const { return m_Sampler; }

		/**
		 * Get the current image layout.
		 *
		 * @return The image layout.
		 */
		VkImageLayout layout() const { return m_CurrentLayout; }

		/**
		 * Get the image aspect flags.
		 *
		 * @return The aspect flags.
		 */
		VkImageAspectFlags getImageAspectFlags() const;

		/**
		 * Get the pixel size of the image.
		 *
		 * @return The pixel size.
		 */
		uint8_t getPixelSize() const;

	private:
		GraphicsEngine& m_Engine;

		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;

		VmaAllocation m_Allocation = nullptr;

		const VkExtent3D m_Extent;
		const VkFormat m_Format = VK_FORMAT_UNDEFINED;
		const VkImageUsageFlags m_Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT| VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	};
}