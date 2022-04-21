// Copyright (c) 2022 Dhiraj Wishal

#include "ShaderCode.hpp"
#include "Utility.hpp"

#include <spirv_reflect.h>
#include <spdlog/spdlog.h>
#include <fstream>

namespace
{
	/**
	 * Validate the reflection result.
	 *
	 * @param result The reflection result.
	 */
	void ValidateReflection(const SpvReflectResult result)
	{
		switch (result)
		{
		case SPV_REFLECT_RESULT_SUCCESS:										return;
		case SPV_REFLECT_RESULT_NOT_READY:										spdlog::error("Shader not ready!"); break;
		case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED:								spdlog::error("Shader parse failed!"); break;
		case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:								spdlog::error("Shader allocation failed!"); break;
		case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:							spdlog::error("Shader range exceeded!"); break;
		case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:								spdlog::error("Shader null pointer!"); break;
		case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:							spdlog::error("Shader internal reflection error!"); break;
		case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:							spdlog::error("Shader count mismatch!"); break;
		case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:						spdlog::error("Shader element not found!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:					spdlog::error("Shader invalid SPIRV code size!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER:				spdlog::error("Shader invalid SPIRV magic number!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:						spdlog::error("Shader SPIRV unexpected end of file (EOF)!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE:				spdlog::error("Shader invalid SPIRV ID reference!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW:				spdlog::error("Shader invalid SPIRV descriptor set number overflow!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS:				spdlog::error("Shader invalid SPIRV storage class!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:							spdlog::error("Shader invalid SPIRV recursion!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION:				spdlog::error("Shader invalid SPIRV instruction!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA:				spdlog::error("Shader invalid SPIRV block data!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE:		spdlog::error("Shader invalid SPIRV block member reference!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT:				spdlog::error("Shader invalid SPIRV entry point!"); break;
		case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE:				spdlog::error("Shader invalid SPIRV execution mode!"); break;
		default:																spdlog::error("Unknown reflection error!");
		}
	}

	/**
	 * Get the Vulkan descriptor type from the reflection descriptor type.
	 *
	 * @param type The reflection descriptor type.
	 * @return The Vulkan descriptor type.
	 */
	VkDescriptorType GetVkDescriptorType(const SpvReflectDescriptorType type)
	{
		switch (type)
		{
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:							return VK_DESCRIPTOR_TYPE_SAMPLER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:						return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:						return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:				return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:				return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:					return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:					return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:					return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:		return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
		default:															spdlog::error("Invalid shader descriptor type!");
		}

		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	}

}

namespace rapid
{
	ShaderCode::ShaderCode(std::filesystem::path&& file, VkShaderStageFlags stageFlags)
		: m_FilePath(std::move(file)), m_Flags(stageFlags)
	{
		loadShaderCode();
		performReflection();
	}

	VkShaderModule ShaderCode::createModule(GraphicsEngine& engine) const
	{
		VkShaderModuleCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.codeSize = m_ShaderCode.size(),
			.pCode = m_ShaderCode.data(),
		};

		VkShaderModule shaderModule = VK_NULL_HANDLE;
		utility::ValidateResult(engine.getDeviceTable().vkCreateShaderModule(engine.getLogicalDevice(), &createInfo, nullptr, &shaderModule), "Failed to create the shader module!");

		return shaderModule;
	}

	void ShaderCode::loadShaderCode()
	{
		// Load the shader file.
		std::fstream shaderFile(m_FilePath, std::ios::in | std::ios::binary | std::ios::ate);

		if (!shaderFile.is_open())
		{
			spdlog::error("Failed to load the shader file! File path: {}", m_FilePath.string());
			return;
		}

		// Get the file size.
		const auto size = shaderFile.tellg();
		shaderFile.seekg(0);

		// Load its content.
		m_ShaderCode.resize(size);
		shaderFile.read(reinterpret_cast<char*>(m_ShaderCode.data()), size);
		shaderFile.close();
	}

	void ShaderCode::performReflection()
	{
		SpvReflectShaderModule shaderModule = {};
		uint32_t variableCount = 0;

		const auto shaderCode = std::vector<uint32_t>(m_ShaderCode.begin(), m_ShaderCode.begin() + (m_ShaderCode.size() / 4));
		ValidateReflection(spvReflectCreateShaderModule(shaderCode.size() * sizeof(uint32_t), shaderCode.data(), &shaderModule));

		// Resolve shader inputs.
		{
			ValidateReflection(spvReflectEnumerateInputVariables(&shaderModule, &variableCount, nullptr));

			std::vector<SpvReflectInterfaceVariable*> pInputs(variableCount);
			ValidateReflection(spvReflectEnumerateInputVariables(&shaderModule, &variableCount, pInputs.data()));

			m_InputAttributes.reserve(pInputs.size());

			// Iterate through the attributes and load them.
			for (auto& resource : pInputs)
			{
				if (resource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED)
					continue;

				// If the type is not a built in one, we can proceed to add them.
				if (resource->built_in == -1)
				{
					ShaderAttribute attribute;

					if (resource->name)
						attribute.m_Name = resource->name;

					attribute.m_Location = resource->location;
					attribute.m_Size = (resource->type_description->traits.numeric.scalar.width / 8) *
						std::max(resource->type_description->traits.numeric.vector.component_count, uint32_t(1));

					m_InputAttributes.emplace_back(attribute);
				}
			}
		}

		// Sort the inputs.
		std::sort(m_InputAttributes.begin(), m_InputAttributes.end(), [](const ShaderAttribute& lhs, const ShaderAttribute& rhs) { return lhs.m_Location < rhs.m_Location; });

		// Resolve shader outputs.
		{
			ValidateReflection(spvReflectEnumerateOutputVariables(&shaderModule, &variableCount, nullptr));

			std::vector<SpvReflectInterfaceVariable*> pOutputs(variableCount);
			ValidateReflection(spvReflectEnumerateOutputVariables(&shaderModule, &variableCount, pOutputs.data()));

			m_OutputAttributes.reserve(pOutputs.size());

			// Iterate through the attributes and load them.
			for (auto& resource : pOutputs)
			{
				if (resource->format == SpvReflectFormat::SPV_REFLECT_FORMAT_UNDEFINED)
					continue;

				// If the type is not a built in one, we can proceed to add them.
				if (resource->built_in == -1)
				{
					ShaderAttribute attribute;

					if (resource->name)
						attribute.m_Name = resource->name;

					attribute.m_Location = resource->location;
					attribute.m_Size = (resource->type_description->traits.numeric.scalar.width / 8) *
						std::max(resource->type_description->traits.numeric.vector.component_count, uint32_t(1));

					m_OutputAttributes.emplace_back(attribute);
				}
			}
		}

		// Sort the outputs.
		std::sort(m_OutputAttributes.begin(), m_OutputAttributes.end(), [](const ShaderAttribute& lhs, const ShaderAttribute& rhs) { return lhs.m_Location < rhs.m_Location; });

		// Resolve uniforms.
		{
			ValidateReflection(spvReflectEnumerateDescriptorBindings(&shaderModule, &variableCount, nullptr));

			std::vector<SpvReflectDescriptorBinding*> pBindings(variableCount);
			ValidateReflection(spvReflectEnumerateDescriptorBindings(&shaderModule, &variableCount, pBindings.data()));

			m_LayoutBindings.reserve(variableCount);

			VkDescriptorSetLayoutBinding vBinding = {};
			vBinding.stageFlags = m_Flags;
			vBinding.pImmutableSamplers = VK_NULL_HANDLE;

			// Iterate over the resources and setup the bindings.
			for (auto& resource : pBindings)
			{
				vBinding.descriptorType = GetVkDescriptorType(resource->descriptor_type);
				vBinding.descriptorCount = resource->count;
				vBinding.binding = resource->binding;
				m_LayoutBindings.emplace_back(vBinding);

				ShaderBinding binding;
				binding.m_Binding = resource->binding;
				binding.m_Set = resource->set;
				binding.m_Count = resource->count;
				binding.m_Type = vBinding.descriptorType;

				m_Bindings[resource->name] = binding;
			}
		}

		// Resolve push constants.
		{
			ValidateReflection(spvReflectEnumeratePushConstantBlocks(&shaderModule, &variableCount, nullptr));

			std::vector<SpvReflectBlockVariable*> pPushConstants(variableCount);
			ValidateReflection(spvReflectEnumeratePushConstantBlocks(&shaderModule, &variableCount, pPushConstants.data()));

			VkPushConstantRange vPushConstantRange = {};
			vPushConstantRange.stageFlags = m_Flags;
			vPushConstantRange.offset = 0;

			// Iterate over the push constants and setup.
			for (auto& resource : pPushConstants)
			{
				vPushConstantRange.size = resource->size;
				vPushConstantRange.offset = resource->offset;
				m_PushConstants.emplace_back(vPushConstantRange);
			}
		}
	}
}