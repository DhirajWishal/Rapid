// Copyright (c) 2022 Dhiraj Wishal

#include "ShaderResource.hpp"

namespace rapid
{
	ShaderResource::ShaderResource(GraphicsEngine& engine, VkDescriptorSetLayout layout, VkDescriptorSet set)
		: m_Engine(engine), m_vDescriptorSetLayout(layout), m_DescriptorSet(set)
	{
	}

	void ShaderResource::update(VkDescriptorSet set)
	{
		std::vector<VkCopyDescriptorSet> vCopies;
		vCopies.reserve(m_Bindings.size());

		VkCopyDescriptorSet copyDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
			.pNext = nullptr,
			.srcSet = m_DescriptorSet,
			.srcArrayElement = 0,
			.dstSet = set,
			.dstArrayElement = 0,
			.descriptorCount = 1
		};

		// Iterate through the bindings and setup the copy info.
		for (const auto binding : m_Bindings)
		{
			copyDescriptorSet.dstBinding = binding;
			copyDescriptorSet.srcBinding = binding;

			vCopies.emplace_back(copyDescriptorSet);
		}

		m_Engine.getDeviceTable().vkUpdateDescriptorSets(m_Engine.getLogicalDevice(), 0, nullptr, static_cast<uint32_t>(vCopies.size()), vCopies.data());
		m_DescriptorSet = set;
	}

	void ShaderResource::bindResource(uint32_t location, const Buffer& buffer)
	{
		VkDescriptorBufferInfo bufferInfo = {
			.buffer = buffer.buffer(),
			.offset = 0,
			.range = buffer.size()
		};

		VkWriteDescriptorSet writeDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = m_DescriptorSet,
			.dstBinding = location,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pImageInfo = nullptr,
			.pBufferInfo = &bufferInfo,
			.pTexelBufferView = nullptr
		};

		m_Engine.getDeviceTable().vkUpdateDescriptorSets(m_Engine.getLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
		m_Bindings.insert(location);
	}

	void ShaderResource::bindResource(uint32_t location, const Image& image)
	{
		VkDescriptorImageInfo imageInfo = {
			.sampler = image.getSampler(),
			.imageView = image.getImageView(),
			.imageLayout = image.layout()
		};

		VkWriteDescriptorSet writeDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = m_DescriptorSet,
			.dstBinding = location,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &imageInfo,
			.pBufferInfo = nullptr,
			.pTexelBufferView = nullptr
		};

		m_Engine.getDeviceTable().vkUpdateDescriptorSets(m_Engine.getLogicalDevice(), 1, &writeDescriptorSet, 0, nullptr);
		m_Bindings.insert(location);
	}
}