// Copyright (c) 2022 Dhiraj Wishal

#include "GraphicsPipeline.hpp"
#include "Utility.hpp"

#include <spdlog/spdlog.h>
#include <fstream>

namespace
{
	/**
	 * Get the stage flag bits from the flags.
	 *
	 * @param flags The shader flags.
	 * @return The shader flag bits.
	 */
	VkShaderStageFlagBits GetStageFlagBits(const VkShaderStageFlags flags)
	{
		if (flags & VK_SHADER_STAGE_VERTEX_BIT)
			return VK_SHADER_STAGE_VERTEX_BIT;

		if (flags & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

		if (flags & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

		if (flags & VK_SHADER_STAGE_GEOMETRY_BIT)
			return VK_SHADER_STAGE_GEOMETRY_BIT;

		if (flags & VK_SHADER_STAGE_FRAGMENT_BIT)
			return VK_SHADER_STAGE_FRAGMENT_BIT;

		spdlog::error("Invalid or unsupported shader stage flags!");
		return VK_SHADER_STAGE_ALL;
	}

	/**
	 * Get the Vulkan format from attribute size.
	 *
	 * @pram size The attribute size.
	 * @return The format.
	 */
	VkFormat GetFormatFromSize(const uint32_t size)
	{
		switch (size)
		{
		case 4:
			return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;

		case 8:
			return VkFormat::VK_FORMAT_R32G32_SFLOAT;

		case 12:
			return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;

		case 16:
			return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;

		default:
			spdlog::error("Invalid or unsupported shader attribute type size!");
		}

		return VkFormat::VK_FORMAT_UNDEFINED;
	}

	/**
	 * Create a file if it does not exist.
	 *
	 * @param path The file.
	 */
	void CreateIfNotExist(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			std::fstream file(path, std::ios::out);
			file.close();
		}
	}
}

namespace rapid
{
	GraphicsPipeline::GraphicsPipeline(GraphicsEngine& engine, Window& window, std::filesystem::path&& cache, const ShaderCode& vertex, const ShaderCode& fragment)
		: m_CacheFile(std::move(cache)), m_ShaderCode({ vertex, fragment }), m_Engine(engine), m_Window(window)
	{
		// Create one binding blob.
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings(vertex.m_LayoutBindings.begin(), vertex.m_LayoutBindings.end());
		layoutBindings.insert(layoutBindings.end(), fragment.m_LayoutBindings.begin(), fragment.m_LayoutBindings.end());

		std::unordered_map<std::string, ShaderBinding> bindings(vertex.m_Bindings.begin(), vertex.m_Bindings.end());
		bindings.insert(fragment.m_Bindings.begin(), fragment.m_Bindings.end());

		m_DescriptorPoolSizes.reserve(bindings.size());
		for (const auto& [name, binding] : bindings)
		{
			VkDescriptorPoolSize vPoolSize = {
				.type = binding.m_Type,
				.descriptorCount = binding.m_Count
			};

			m_DescriptorPoolSizes.emplace_back(vPoolSize);
		}

		// Now we can setup the descriptor set layout.
		setupDescriptorSetLayout(std::move(layoutBindings));

		// Resolve push constants and create the layout.
		std::vector<VkPushConstantRange> pushConstants(vertex.m_PushConstants.begin(), vertex.m_PushConstants.end());
		pushConstants.insert(pushConstants.end(), fragment.m_PushConstants.begin(), fragment.m_PushConstants.end());

		createPipelineLayout(std::move(pushConstants));

		// Load pipeline cache and creat the pipeline.
		loadPipelineCache();
		createPipeline();
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
		if (isActive())
			terminate();
	}

	void GraphicsPipeline::terminate()
	{
		m_Engine.getDeviceTable().vkDestroyPipeline(m_Engine.getLogicalDevice(), m_Pipeline, nullptr);
		m_Engine.getDeviceTable().vkDestroyPipelineCache(m_Engine.getLogicalDevice(), m_PipelineCache, nullptr);
		m_Engine.getDeviceTable().vkDestroyPipelineLayout(m_Engine.getLogicalDevice(), m_PipelineLayout, nullptr);
		m_Engine.getDeviceTable().vkDestroyDescriptorSetLayout(m_Engine.getLogicalDevice(), m_DescriptorSetLayout, nullptr);
		m_Engine.getDeviceTable().vkDestroyDescriptorPool(m_Engine.getLogicalDevice(), m_DescriptorPool, nullptr);

		m_IsTerminated = true;
	}

	void GraphicsPipeline::recreate()
	{
		m_Engine.getDeviceTable().vkDestroyPipeline(m_Engine.getLogicalDevice(), m_Pipeline, nullptr);
		createPipeline();
	}

	ShaderResource& GraphicsPipeline::createShaderResource()
	{
		// First, let's create a new descriptor pool.
		VkDescriptorPoolCreateInfo poolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.maxSets = static_cast<uint32_t>(m_ShaderResources.size()) + 1,
			.poolSizeCount = static_cast<uint32_t>(m_DescriptorPoolSizes.size()),
			.pPoolSizes = m_DescriptorPoolSizes.data()
		};

		VkDescriptorPool vDescriptorPool = VK_NULL_HANDLE;
		utility::ValidateResult(m_Engine.getDeviceTable().vkCreateDescriptorPool(m_Engine.getLogicalDevice(), &poolCreateInfo, nullptr, &vDescriptorPool), "Failed to create the descriptor pool!");

		// Allocate the new descriptor sets.
		VkDescriptorSetAllocateInfo allocateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext = nullptr,
			.descriptorPool = vDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &m_DescriptorSetLayout
		};

		// Now for every resource, we create the new descriptor set and update it.
		for (auto& resource : m_ShaderResources)
		{
			VkDescriptorSet vDescriptorSet = VK_NULL_HANDLE;
			utility::ValidateResult(m_Engine.getDeviceTable().vkAllocateDescriptorSets(m_Engine.getLogicalDevice(), &allocateInfo, &vDescriptorSet), "Failed to allocate descriptor set!");

			resource->update(vDescriptorSet);
		}

		// Destroy the old pool and assign the new one.
		m_Engine.getDeviceTable().vkDestroyDescriptorPool(m_Engine.getLogicalDevice(), m_DescriptorPool, nullptr);
		m_DescriptorPool = vDescriptorPool;

		// Finally, lets create the new descriptor set, assign it to the resource and return its reference.
		VkDescriptorSet vDescriptorSet = VK_NULL_HANDLE;
		utility::ValidateResult(m_Engine.getDeviceTable().vkAllocateDescriptorSets(m_Engine.getLogicalDevice(), &allocateInfo, &vDescriptorSet), "Failed to allocate descriptor set!");
		return *m_ShaderResources.emplace_back(std::make_unique<ShaderResource>(m_Engine, m_DescriptorSetLayout, vDescriptorSet));
	}

	void GraphicsPipeline::setupDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>&& bindings)
	{
		// Create the descriptor set layout.
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(bindings.size()),
			.pBindings = bindings.data()
		};

		utility::ValidateResult(m_Engine.getDeviceTable().vkCreateDescriptorSetLayout(m_Engine.getLogicalDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout), "Failed to create the descriptor set layout!");
	}

	void GraphicsPipeline::createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants)
	{
		VkPipelineLayoutCreateInfo layoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0,
			.setLayoutCount = 1,
			.pSetLayouts = &m_DescriptorSetLayout,
			.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size()),
			.pPushConstantRanges = pushConstants.data(),
		};

		utility::ValidateResult(m_Engine.getDeviceTable().vkCreatePipelineLayout(m_Engine.getLogicalDevice(), &layoutCreateInfo, nullptr, &m_PipelineLayout), "Failed to create the pipeline layout!");
	}

	void GraphicsPipeline::loadPipelineCache()
	{
		// Check to make sure that we have the file.
		CreateIfNotExist(m_CacheFile);

		// Load data from file.
		std::fstream cacheFile(m_CacheFile, std::ios::in | std::ios::ate | std::ios::binary);

		// If file does not exist, return without an issue.
		if (!cacheFile.is_open())
		{
			spdlog::error("Failed to load the cache file! Given path: {}", m_CacheFile.string());
			return;
		}

		const uint64_t size = cacheFile.tellg();
		cacheFile.seekg(0);

		auto buffer = std::make_unique<uint8_t[]>(size);
		cacheFile.read(reinterpret_cast<char*>(buffer.get()), size);

		cacheFile.close();

		// Create the pipeline cache.
		VkPipelineCacheCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0,
			.initialDataSize = size,
			.pInitialData = buffer.get(),
		};

		utility::ValidateResult(m_Engine.getDeviceTable().vkCreatePipelineCache(m_Engine.getLogicalDevice(), &createInfo, nullptr, &m_PipelineCache), "Failed to create the pipeline cache!");
	}

	void GraphicsPipeline::savePipelineCache()
	{
		// Return if we don't have anything to save.
		if (m_PipelineCache == VK_NULL_HANDLE)
			return;

		// Load cache data.
		size_t cacheSize = 0;
		utility::ValidateResult(m_Engine.getDeviceTable().vkGetPipelineCacheData(m_Engine.getLogicalDevice(), m_PipelineCache, &cacheSize, nullptr), "Failed to get the pipeline cache size!");

		auto buffer = std::make_unique<uint8_t[]>(cacheSize);
		utility::ValidateResult(m_Engine.getDeviceTable().vkGetPipelineCacheData(m_Engine.getLogicalDevice(), m_PipelineCache, &cacheSize, buffer.get()), "Failed to get the pipeline cache data!");

		// Write to file.
		std::fstream cacheFile(m_CacheFile, std::ios::out | std::ios::binary);

		if (!cacheFile.is_open())
		{
			spdlog::error("Failed to load the cache file! Given path: {}", m_CacheFile.string());
			return;
		}

		cacheFile.write(reinterpret_cast<char*>(buffer.get()), cacheSize);
		cacheFile.flush();
		cacheFile.close();
	}

	void GraphicsPipeline::createPipeline()
	{
		// Resolve shader info.
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
		shaderStageCreateInfos.reserve(m_ShaderCode.size());

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		VkVertexInputBindingDescription bindingDescription = {
			.binding = 0,
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};

		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pName = "main",
			.pSpecializationInfo = nullptr
		};

		// Iterate over the shaders and resolve information.
		for (const auto& shader : m_ShaderCode)
		{
			shaderStageCreateInfo.module = shader.createModule(m_Engine);
			shaderStageCreateInfo.stage = GetStageFlagBits(shader.m_Flags);

			shaderStageCreateInfos.emplace_back(shaderStageCreateInfo);

			// Resolve inputs if its the vertex shader.
			if (shaderStageCreateInfo.stage == VK_SHADER_STAGE_VERTEX_BIT)
			{
				const auto& inputs = shader.m_InputAttributes;
				attributeDescriptions.reserve(inputs.size());

				VkVertexInputAttributeDescription attributeDescription = {
					.binding = 0,
					.offset = 0
				};

				// Resolve the individual attributes.
				for (const auto& attribute : inputs)
				{
					attributeDescription.location = attribute.m_Location;
					attributeDescription.format = GetFormatFromSize(attribute.m_Size);

					attributeDescriptions.emplace_back(attributeDescription);
					attributeDescription.offset += attribute.m_Size;
				}

				bindingDescription.stride = attributeDescription.offset;
			}
		}

		// Setup vertex input state.
		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &bindingDescription,
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
			.pVertexAttributeDescriptions = attributeDescriptions.data()
		};

		// Setup input assembly state.
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		// Setup tessellation state.
		VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.patchControlPoints = 0
		};

		// Resolve viewport state.
		VkRect2D rect2D = {
			.offset = { 0, 0 },
			.extent = m_Window.extent()
		};

		VkViewport viewport = {
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(rect2D.extent.width),
			.height = static_cast<float>(rect2D.extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &rect2D
		};

		// Setup color blend state.
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_CLEAR,
			.attachmentCount = 1,
			.pAttachments = &colorBlendAttachmentState,
			.blendConstants = { 0.0, 0.0, 0.0, 0.0 }
		};

		// Setup rasterization state.
		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f,
			.lineWidth = 1.0f
		};

		// Setup multisample state.
		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_TRUE,
			.minSampleShading = 1.0f,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
			//.pSampleMask;	// TODO
		};

		// Setup depth stencil state.
		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
			.front = {
				.compareOp = VK_COMPARE_OP_NEVER,
			},
			.back = {
				.compareOp = VK_COMPARE_OP_ALWAYS,
			}
		};

		// Setup dynamic state.
		std::array<VkDynamicState, 2> dynamicStates;
		dynamicStates[0] = VK_DYNAMIC_STATE_SCISSOR;
		dynamicStates[1] = VK_DYNAMIC_STATE_VIEWPORT;

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates = dynamicStates.data()
		};

		// Setup pipeline create info.
		VkGraphicsPipelineCreateInfo pipeineCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size()),
			.pStages = shaderStageCreateInfos.data(),
			.pVertexInputState = &vertexInputStateCreateInfo,
			.pInputAssemblyState = &inputAssemblyStateCreateInfo,
			.pTessellationState = &tessellationStateCreateInfo,
			.pViewportState = &viewportStateCreateInfo,
			.pRasterizationState = &rasterizationStateCreateInfo,
			.pMultisampleState = &multisampleStateCreateInfo,
			.pDepthStencilState = &depthStencilStateCreateInfo,
			.pColorBlendState = &colorBlendStateCreateInfo,
			.pDynamicState = &dynamicStateCreateInfo,
			.layout = m_PipelineLayout,
			.renderPass = m_Window.getRenderPass(),
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = 0
		};

		utility::ValidateResult(m_Engine.getDeviceTable().vkCreateGraphicsPipelines(m_Engine.getLogicalDevice(), m_PipelineCache, 1, &pipeineCreateInfo, nullptr, &m_Pipeline), "Failed to create the graphics pipeline!");

		// Destroy the shader modules because we no longer need them.
		for (auto& stage : shaderStageCreateInfos)
			m_Engine.getDeviceTable().vkDestroyShaderModule(m_Engine.getLogicalDevice(), stage.module, nullptr);

		// Save the pipeline cache.
		savePipelineCache();
	}
}