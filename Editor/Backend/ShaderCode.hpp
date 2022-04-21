// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <vulkan/vulkan.h>

#include <filesystem>
#include <vector>
#include <cstdint>
#include <unordered_map>

namespace rapid
{
	/**
	 * Shader binding structure.
	 * This describes a single binding in the shader.
	 */
	struct ShaderBinding
	{
		uint32_t m_Set = 0;
		uint32_t m_Binding = 0;
		uint32_t m_Count = 0;
		VkDescriptorType m_Type = VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM;
	};

	/**
	 * Shader attribute structure.
	 * Shader attributes are of two types, inputs and outputs.
	 */
	struct ShaderAttribute
	{
		std::string m_Name;
		uint32_t m_Location = 0;
		uint32_t m_Size = 0;
	};

	/**
	 * Shader code structure.
	 * This object holds information about a single SPIR-V file.
	 */
	struct ShaderCode final
	{
		/**
		 * Explicit constructor.
		 *
		 * @param file The shader source file.
		 * @param stageFlags The shader stage flags.
		 */
		explicit ShaderCode(std::filesystem::path&& file, VkShaderStageFlags stageFlags);

		std::vector<uint32_t> m_ShaderCode;
		std::filesystem::path m_FilePath;

		std::unordered_map<std::string, ShaderBinding> m_Bindings;
		std::vector<ShaderAttribute> m_InputAttributes;
		std::vector<ShaderAttribute> m_OutputAttributes;
		std::vector<VkPushConstantRange> m_PushConstants;

		const VkShaderStageFlags m_Flags;

	private:
		/**
		 * Load the shader code.
		 */
		void loadShaderCode();

		/**
		 * Perform reflection over the shader code.
		 */
		void performReflection();
	};
}