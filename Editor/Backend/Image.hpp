// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Buffer.hpp"

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
		 * @param extent The image extent.
		 * @param format The image format.
		 */
		explicit Image(GraphicsEngine& engine, VkExtent3D extent, VkFormat format);

		/**
		 * Explicit constructor.
		 *
		 * @param engine The graphics engine.
		 * @param extent The image extent.
		 * @param format The image format.
		 * @param pImageData The image data to copy.
		 */
		explicit Image(GraphicsEngine& engine, VkExtent3D extent, VkFormat format, const std::byte* pImageData);

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
		 * Get the size of the image.
		 *
		 * @return The size.
		 */
		uint64_t size() const { return static_cast<uint64_t>(m_Extent.width) * m_Extent.height * m_Extent.depth * getPixelSize(); }

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
		VkImage getImage() const { return m_Image; }

		/**
		 * Get the image view.
		 *
		 * @return The image view.
		 */
		VkImageView getImageView() const { return m_ImageView; }

		/**
		 * Get the image sampler.
		 *
		 * @return The sampler.
		 */
		VkSampler getSampler() const { return m_Sampler; }

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
		/**
		 * Create the image.
		 */
		void createImage();

		/**
		 * Create the image view.
		 */
		void createImageview();

		/**
		 * Create the image sampler.
		 */
		void createSampler();

	private:
		GraphicsEngine& m_Engine;

		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;

		VmaAllocation m_Allocation = nullptr;

		const VkExtent3D m_Extent;
		const VkFormat m_Format = VK_FORMAT_UNDEFINED;
		const VkImageUsageFlags m_Usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		VkImageLayout m_CurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	};
}