// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Window.hpp"
#include "ShaderCode.hpp"
#include "ShaderResource.hpp"

namespace rapid
{
	/**
	 * Graphics pipeline object.
	 * This object is used to render objects.
	 *
	 * Note that when providing shaders, all descriptors, throughout the shaders, should use set = 0.
	 */
	class GraphicsPipeline final : public BackendObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The graphic engine.
		 * @param window The window which owns the pipeline.
		 * @param cache The cache file name.
		 * @param vertex The vertex shader code.
		 * @param fragment The fragment shader code.
		 */
		explicit GraphicsPipeline(GraphicsEngine& engine, Window& window, std::filesystem::path&& cache, const ShaderCode& vertex, const ShaderCode& fragment);

		/**
		 * Destructor.
		 */
		~GraphicsPipeline();

		/**
		 * Terminate the pipeline.
		 */
		void terminate() override;

		/**
		 * Create a new shader resource.
		 */
		ShaderResource& createShaderResource();

	private:
		/**
		 * Setup descriptors.
		 *
		 * @param bindings The layout bindings.
		 */
		void setupDescriptors(std::vector<VkDescriptorSetLayoutBinding>&& bindings);

		/**
		 * Create the pipeline layout.
		 *
		 * @param pushConstants The push constants.
		 */
		void createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants);

		/**
		 * Load pipeline cache.
		 */
		void loadPipelineCache();

		/**
		 * Save the pipeline cache.
		 */
		void savePipelineCache();

		/**
		 * Create the pipeline.
		 */
		void createPipeline();

	private:
		std::filesystem::path m_CacheFile;
		std::vector<ShaderCode> m_ShaderCode = {};	// This is not the best move, but we need it for pipeline re-creation.
		std::vector<VkDescriptorPoolSize> m_DescriptorPoolSizes = {};
		std::vector<ShaderResource> m_ShaderResources = {};

		GraphicsEngine& m_Engine;
		Window& m_Window;

		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
	};
}