// Copyright (c) 2022 Dhiraj Wishal

#include "Image.hpp"
#include "Utility.hpp"  

#include <spdlog/spdlog.h>

namespace
{
	/**
	 * Get the pipeline stage flags from access flags.
	 *
	 * @param flags Access flags.
	 * @return The stage flags.
	 */
	VkPipelineStageFlags GetPipelineStageFlags(const VkAccessFlags flags)
	{
		switch (flags)
		{
		case VK_ACCESS_INDIRECT_COMMAND_READ_BIT:						return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		case VK_ACCESS_INDEX_READ_BIT:									return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		case VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT:						return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		case VK_ACCESS_UNIFORM_READ_BIT:								return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		case VK_ACCESS_SHADER_READ_BIT:									return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		case VK_ACCESS_SHADER_WRITE_BIT:								return VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		case VK_ACCESS_INPUT_ATTACHMENT_READ_BIT:						return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		case VK_ACCESS_COLOR_ATTACHMENT_READ_BIT:						return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		case VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT:						return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT:				return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:				return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		case VK_ACCESS_TRANSFER_READ_BIT:								return VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		case VK_ACCESS_TRANSFER_WRITE_BIT:								return VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		case VK_ACCESS_HOST_READ_BIT:									return VK_PIPELINE_STAGE_HOST_BIT;
		case VK_ACCESS_HOST_WRITE_BIT:									return VK_PIPELINE_STAGE_HOST_BIT;
		case VK_ACCESS_MEMORY_READ_BIT:									return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		case VK_ACCESS_MEMORY_WRITE_BIT:								return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		case VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT:		return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		case VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV:					return VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
		case VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV:					return VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
		case VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT:				return VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
		case VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR:	return VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
		case VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT:				return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
		case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT:		return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
		case VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT:			return VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		case VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR:				return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		case VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR:			return VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		case VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT:				return VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
		default:														return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		}
	}
}

namespace rapid
{
	Image::Image(GraphicsEngine& engine, VkExtent3D extent, VkFormat format)
		: m_Engine(engine), m_Extent(extent), m_Format(format)
	{
		// Set up all the primitives.
		createImage();
		createImageview();
		createSampler();
	}

	Image::Image(GraphicsEngine& engine, VkExtent3D extent, VkFormat format, const std::byte* pImageData)
		: m_Engine(engine), m_Extent(extent), m_Format(format)
	{
		// Set up all the primitives.
		createImage();
		createImageview();
		createSampler();

		// Copy the image data to the image.
		auto stagingBuffer = Buffer(m_Engine, size(), BufferType::Staging);
		auto pBufferMemory = stagingBuffer.mapMemory();

		std::copy(pImageData, pImageData + size(), pBufferMemory);
		stagingBuffer.unmapMemory();

		fromBuffer(stagingBuffer);
	}

	Image::~Image()
	{
		if (isActive())
			terminate();
	}

	void Image::terminate()
	{
		vkDestroySampler(m_Engine.getLogicalDevice(), m_Sampler, nullptr);
		vkDestroyImageView(m_Engine.getLogicalDevice(), m_ImageView, nullptr);
		vmaDestroyImage(m_Engine.getAllocator(), m_Image, m_Allocation);
		m_IsTerminated = true;
	}

	void Image::changeImageLayout(const VkImageLayout newLayout, const VkCommandBuffer vCommandBuffer)
	{
		// Create the memory barrier.
		VkImageMemoryBarrier memorybarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = 0,
			.dstAccessMask = 0,
			.oldLayout = m_CurrentLayout,
			.newLayout = newLayout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = m_Image,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
		};

		// Resolve the source access masks.
		switch (m_CurrentLayout)
		{
		case VK_IMAGE_LAYOUT_GENERAL:
		case VK_IMAGE_LAYOUT_UNDEFINED:
			memorybarrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			memorybarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			memorybarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			memorybarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
			memorybarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			memorybarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			//vMB.srcAccessMask = VK_ACCESS_;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			memorybarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			memorybarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;

		default:
			spdlog::error("Unsupported layout transition!");
			return;
		}

		// Resolve the destination access masks.
		switch (newLayout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
		case VK_IMAGE_LAYOUT_GENERAL:
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			memorybarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			memorybarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			memorybarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			memorybarrier.dstAccessMask = memorybarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			memorybarrier.srcAccessMask |= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			memorybarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;

		default:
			spdlog::error("Unsupported layout transition!");
			return;
		}

		// Resolve the pipeline stages.
		const auto sourceStage = GetPipelineStageFlags(memorybarrier.srcAccessMask);
		const auto destinationStage = GetPipelineStageFlags(memorybarrier.dstAccessMask);

		// Issue the commands. 
		// Here we begin the buffer recording if a command buffer was not given.
		if (vCommandBuffer == VK_NULL_HANDLE)
		{
			m_Engine.getDeviceTable().vkCmdPipelineBarrier(m_Engine.beginCommandBufferRecording(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &memorybarrier);
			m_Engine.executeRecordedCommands();
		}
		else
			m_Engine.getDeviceTable().vkCmdPipelineBarrier(vCommandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &memorybarrier);

		m_CurrentLayout = newLayout;
	}

	void Image::fromBuffer(const Buffer& buffer)
	{
		VkBufferImageCopy imageCopy = {
			.bufferOffset = 0,
			.bufferRowLength = m_Extent.width,
			.bufferImageHeight = m_Extent.height,
			.imageSubresource = {
				.aspectMask = getImageAspectFlags(),
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.imageOffset = {},
			.imageExtent = m_Extent,
		};

		const auto oldlayout = m_CurrentLayout;
		const auto vCommandBuffer = m_Engine.beginCommandBufferRecording();

		// Change the layout to transfer source
		changeImageLayout(VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vCommandBuffer);

		// Copy the image.
		m_Engine.getDeviceTable().vkCmdCopyBufferToImage(vCommandBuffer, buffer.buffer(), m_Image, m_CurrentLayout, 1, &imageCopy);

		// Get it back to the old layout.
		if (oldlayout != VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED && oldlayout != VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED)
			changeImageLayout(oldlayout, vCommandBuffer);

		// Execute the commands.
		m_Engine.executeRecordedCommands();
	}

	std::unique_ptr<Buffer> Image::toBuffer()
	{
		auto pBuffer = std::make_unique<Buffer>(m_Engine, size(), BufferType::Staging);

		VkBufferImageCopy vImageCopy = {};
		vImageCopy.imageExtent = m_Extent;
		vImageCopy.imageOffset = {};
		vImageCopy.imageSubresource.aspectMask = getImageAspectFlags();
		vImageCopy.imageSubresource.baseArrayLayer = 0;
		vImageCopy.imageSubresource.layerCount = 1;
		vImageCopy.imageSubresource.mipLevel = 0;
		vImageCopy.bufferOffset = 0;
		vImageCopy.bufferRowLength = m_Extent.width;
		vImageCopy.bufferImageHeight = m_Extent.height;

		const auto oldlayout = m_CurrentLayout;
		const auto vCommandBuffer = m_Engine.beginCommandBufferRecording();

		// Change the layout to transfer source
		changeImageLayout(VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vCommandBuffer);

		// Copy the image.
		m_Engine.getDeviceTable().vkCmdCopyImageToBuffer(vCommandBuffer, m_Image, m_CurrentLayout, pBuffer->buffer(), 1, &vImageCopy);

		// Get it back to the old layout.
		if (oldlayout != VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED && oldlayout != VkImageLayout::VK_IMAGE_LAYOUT_PREINITIALIZED)
			changeImageLayout(oldlayout, vCommandBuffer);

		// Execute the commands.
		m_Engine.executeRecordedCommands();

		return pBuffer;
	}

	VkImageAspectFlags Image::getImageAspectFlags() const
	{
		if (m_Usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			return VK_IMAGE_ASPECT_DEPTH_BIT;

		else if (
			m_Usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ||
			m_Usage & VK_IMAGE_USAGE_SAMPLED_BIT ||
			m_Usage & VK_IMAGE_USAGE_STORAGE_BIT)
			return VK_IMAGE_ASPECT_COLOR_BIT;

		return 0;
	}

	uint8_t Image::getPixelSize() const
	{
		switch (m_Format)
		{
		case VkFormat::VK_FORMAT_R8_UNORM:
		case VkFormat::VK_FORMAT_R8_SNORM:
		case VkFormat::VK_FORMAT_R8_USCALED:
		case VkFormat::VK_FORMAT_R8_SSCALED:
		case VkFormat::VK_FORMAT_R8_UINT:
		case VkFormat::VK_FORMAT_R8_SINT:
		case VkFormat::VK_FORMAT_R8_SRGB:
		case VkFormat::VK_FORMAT_S8_UINT:
			return 1;

		case VkFormat::VK_FORMAT_R8G8_UNORM:
		case VkFormat::VK_FORMAT_R8G8_SNORM:
		case VkFormat::VK_FORMAT_R8G8_USCALED:
		case VkFormat::VK_FORMAT_R8G8_SSCALED:
		case VkFormat::VK_FORMAT_R8G8_UINT:
		case VkFormat::VK_FORMAT_R8G8_SINT:
		case VkFormat::VK_FORMAT_R8G8_SRGB:
		case VkFormat::VK_FORMAT_R16_UNORM:
		case VkFormat::VK_FORMAT_R16_SNORM:
		case VkFormat::VK_FORMAT_R16_USCALED:
		case VkFormat::VK_FORMAT_R16_SSCALED:
		case VkFormat::VK_FORMAT_R16_UINT:
		case VkFormat::VK_FORMAT_R16_SINT:
		case VkFormat::VK_FORMAT_R16_SFLOAT:
		case VkFormat::VK_FORMAT_D16_UNORM:
			return 2;

		case VkFormat::VK_FORMAT_R8G8B8_UNORM:
		case VkFormat::VK_FORMAT_R8G8B8_SNORM:
		case VkFormat::VK_FORMAT_R8G8B8_USCALED:
		case VkFormat::VK_FORMAT_R8G8B8_SSCALED:
		case VkFormat::VK_FORMAT_R8G8B8_UINT:
		case VkFormat::VK_FORMAT_R8G8B8_SINT:
		case VkFormat::VK_FORMAT_R8G8B8_SRGB:
		case VkFormat::VK_FORMAT_B8G8R8_UNORM:
		case VkFormat::VK_FORMAT_B8G8R8_SNORM:
		case VkFormat::VK_FORMAT_B8G8R8_USCALED:
		case VkFormat::VK_FORMAT_B8G8R8_SSCALED:
		case VkFormat::VK_FORMAT_B8G8R8_UINT:
		case VkFormat::VK_FORMAT_B8G8R8_SINT:
		case VkFormat::VK_FORMAT_B8G8R8_SRGB:
		case VkFormat::VK_FORMAT_D16_UNORM_S8_UINT:
			return 3;

		case VkFormat::VK_FORMAT_R8G8B8A8_UNORM:
		case VkFormat::VK_FORMAT_R8G8B8A8_SNORM:
		case VkFormat::VK_FORMAT_R8G8B8A8_USCALED:
		case VkFormat::VK_FORMAT_R8G8B8A8_SSCALED:
		case VkFormat::VK_FORMAT_R8G8B8A8_UINT:
		case VkFormat::VK_FORMAT_R8G8B8A8_SINT:
		case VkFormat::VK_FORMAT_R8G8B8A8_SRGB:
		case VkFormat::VK_FORMAT_B8G8R8A8_UNORM:
		case VkFormat::VK_FORMAT_B8G8R8A8_SNORM:
		case VkFormat::VK_FORMAT_B8G8R8A8_USCALED:
		case VkFormat::VK_FORMAT_B8G8R8A8_SSCALED:
		case VkFormat::VK_FORMAT_B8G8R8A8_UINT:
		case VkFormat::VK_FORMAT_B8G8R8A8_SINT:
		case VkFormat::VK_FORMAT_B8G8R8A8_SRGB:
		case VkFormat::VK_FORMAT_A8B8G8R8_UNORM_PACK32:
		case VkFormat::VK_FORMAT_A8B8G8R8_SNORM_PACK32:
		case VkFormat::VK_FORMAT_A8B8G8R8_USCALED_PACK32:
		case VkFormat::VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
		case VkFormat::VK_FORMAT_A8B8G8R8_UINT_PACK32:
		case VkFormat::VK_FORMAT_A8B8G8R8_SINT_PACK32:
		case VkFormat::VK_FORMAT_A8B8G8R8_SRGB_PACK32:
		case VkFormat::VK_FORMAT_A2R10G10B10_UNORM_PACK32:
		case VkFormat::VK_FORMAT_A2R10G10B10_SNORM_PACK32:
		case VkFormat::VK_FORMAT_A2R10G10B10_USCALED_PACK32:
		case VkFormat::VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
		case VkFormat::VK_FORMAT_A2R10G10B10_UINT_PACK32:
		case VkFormat::VK_FORMAT_A2R10G10B10_SINT_PACK32:
		case VkFormat::VK_FORMAT_A2B10G10R10_UNORM_PACK32:
		case VkFormat::VK_FORMAT_A2B10G10R10_SNORM_PACK32:
		case VkFormat::VK_FORMAT_A2B10G10R10_USCALED_PACK32:
		case VkFormat::VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
		case VkFormat::VK_FORMAT_A2B10G10R10_UINT_PACK32:
		case VkFormat::VK_FORMAT_A2B10G10R10_SINT_PACK32:
		case VkFormat::VK_FORMAT_R16G16_UNORM:
		case VkFormat::VK_FORMAT_R16G16_SNORM:
		case VkFormat::VK_FORMAT_R16G16_USCALED:
		case VkFormat::VK_FORMAT_R16G16_SSCALED:
		case VkFormat::VK_FORMAT_R16G16_UINT:
		case VkFormat::VK_FORMAT_R16G16_SINT:
		case VkFormat::VK_FORMAT_R16G16_SFLOAT:
		case VkFormat::VK_FORMAT_R32_UINT:
		case VkFormat::VK_FORMAT_R32_SINT:
		case VkFormat::VK_FORMAT_R32_SFLOAT:
		case VkFormat::VK_FORMAT_B10G11R11_UFLOAT_PACK32:
		case VkFormat::VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
		case VkFormat::VK_FORMAT_X8_D24_UNORM_PACK32:
		case VkFormat::VK_FORMAT_D32_SFLOAT:
		case VkFormat::VK_FORMAT_D24_UNORM_S8_UINT:
			return 4;

		case VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT:
			return 5;

		case VkFormat::VK_FORMAT_R16G16B16_UNORM:
		case VkFormat::VK_FORMAT_R16G16B16_SNORM:
		case VkFormat::VK_FORMAT_R16G16B16_USCALED:
		case VkFormat::VK_FORMAT_R16G16B16_SSCALED:
		case VkFormat::VK_FORMAT_R16G16B16_UINT:
		case VkFormat::VK_FORMAT_R16G16B16_SINT:
		case VkFormat::VK_FORMAT_R16G16B16_SFLOAT:
			return 6;

		case VkFormat::VK_FORMAT_R16G16B16A16_UNORM:
		case VkFormat::VK_FORMAT_R16G16B16A16_SNORM:
		case VkFormat::VK_FORMAT_R16G16B16A16_USCALED:
		case VkFormat::VK_FORMAT_R16G16B16A16_SSCALED:
		case VkFormat::VK_FORMAT_R16G16B16A16_UINT:
		case VkFormat::VK_FORMAT_R16G16B16A16_SINT:
		case VkFormat::VK_FORMAT_R16G16B16A16_SFLOAT:
		case VkFormat::VK_FORMAT_R32G32_UINT:
		case VkFormat::VK_FORMAT_R32G32_SINT:
		case VkFormat::VK_FORMAT_R32G32_SFLOAT:
		case VkFormat::VK_FORMAT_R64_UINT:
		case VkFormat::VK_FORMAT_R64_SINT:
		case VkFormat::VK_FORMAT_R64_SFLOAT:
			return 8;

		case VkFormat::VK_FORMAT_R32G32B32_UINT:
		case VkFormat::VK_FORMAT_R32G32B32_SINT:
		case VkFormat::VK_FORMAT_R32G32B32_SFLOAT:
			return 12;

		case VkFormat::VK_FORMAT_R32G32B32A32_UINT:
		case VkFormat::VK_FORMAT_R32G32B32A32_SINT:
		case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT:
		case VkFormat::VK_FORMAT_R64G64_UINT:
		case VkFormat::VK_FORMAT_R64G64_SINT:
		case VkFormat::VK_FORMAT_R64G64_SFLOAT:
			return 16;

		case VkFormat::VK_FORMAT_R64G64B64_UINT:
		case VkFormat::VK_FORMAT_R64G64B64_SINT:
		case VkFormat::VK_FORMAT_R64G64B64_SFLOAT:
			return 24;

		case VkFormat::VK_FORMAT_R64G64B64A64_UINT:
		case VkFormat::VK_FORMAT_R64G64B64A64_SINT:
		case VkFormat::VK_FORMAT_R64G64B64A64_SFLOAT:
			return 32;

		case VkFormat::VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_BC2_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC2_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_BC3_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC3_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_BC4_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC4_SNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC5_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC5_SNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC6H_UFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_BC6H_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_BC7_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_BC7_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_EAC_R11_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_EAC_R11_SNORM_BLOCK:
		case VkFormat::VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
		case VkFormat::VK_FORMAT_G8B8G8R8_422_UNORM:
		case VkFormat::VK_FORMAT_B8G8R8G8_422_UNORM:
		case VkFormat::VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
		case VkFormat::VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
		case VkFormat::VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
		case VkFormat::VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
		case VkFormat::VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
		case VkFormat::VK_FORMAT_R10X6_UNORM_PACK16:
		case VkFormat::VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
		case VkFormat::VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
		case VkFormat::VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
		case VkFormat::VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
		case VkFormat::VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_R12X4_UNORM_PACK16:
		case VkFormat::VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
		case VkFormat::VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
		case VkFormat::VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
		case VkFormat::VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
		case VkFormat::VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G16B16G16R16_422_UNORM:
		case VkFormat::VK_FORMAT_B16G16R16G16_422_UNORM:
		case VkFormat::VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
		case VkFormat::VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
		case VkFormat::VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
		case VkFormat::VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
		case VkFormat::VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
		case VkFormat::VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
		case VkFormat::VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
		case VkFormat::VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
		case VkFormat::VK_FORMAT_A4R4G4B4_UNORM_PACK16:
		case VkFormat::VK_FORMAT_A4B4G4R4_UNORM_PACK16:
		case VkFormat::VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
		case VkFormat::VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
		case VkFormat::VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
		case VkFormat::VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
		case VkFormat::VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
		case VkFormat::VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
		case VkFormat::VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
		case VkFormat::VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
		case VkFormat::VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
		case VkFormat::VK_FORMAT_MAX_ENUM:
			break;
		}

		return 0;
	}

	void Image::createImage()
	{
		VkImageCreateInfo imageCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = m_Format,
			.extent = m_Extent,
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = m_Usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = m_CurrentLayout,
		};

		VmaAllocationCreateInfo allocationCreateInfo = {
			.usage = VMA_MEMORY_USAGE_GPU_ONLY
		};

		utility::ValidateResult(vmaCreateImage(m_Engine.getAllocator(), &imageCreateInfo, &allocationCreateInfo, &m_Image, &m_Allocation, nullptr), "Failed to create the image!");
	}

	void Image::createImageview()
	{
		VkImageViewCreateInfo imageViewCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = m_Image,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = m_Format,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			},
			.subresourceRange = {
				.aspectMask = getImageAspectFlags(),
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};

		utility::ValidateResult(m_Engine.getDeviceTable().vkCreateImageView(m_Engine.getLogicalDevice(), &imageViewCreateInfo, nullptr, &m_ImageView), "Failed to create the image view!");
	}

	void Image::createSampler()
	{
		VkSamplerCreateInfo samplerCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			.mipLodBias = 0.0,
			.anisotropyEnable = VK_TRUE,
			.maxAnisotropy = m_Engine.getPhysicalDeviceProperties().limits.maxSamplerAnisotropy,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0,
			.maxLod = 1,
			.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			.unnormalizedCoordinates = VK_FALSE,
		};

		utility::ValidateResult(m_Engine.getDeviceTable().vkCreateSampler(m_Engine.getLogicalDevice(), &samplerCreateInfo, nullptr, &m_Sampler), "Failed to create the image sampler!");
	}
}