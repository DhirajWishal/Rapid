// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Image.hpp"
#include <set>

namespace rapid
{
	/**
	 * Shader resource class.
	 * This object is used to bind data to a descriptor set and submit to render.
	 */
	class ShaderResource final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine reference.
		 * @param layout The descriptor set layout.
		 * @param set The descriptor set.
		 */
		explicit ShaderResource(GraphicsEngine& engine, VkDescriptorSetLayout layout, VkDescriptorSet set);

		/**
		 * Update the descriptor pool and the descriptor set.
		 * This method is called by the graphics pipeline when a new shader resource object is made.
		 *
		 * This will copy all the existing descriptor data to the new set.
		 *
		 * @param set The new descriptor set.
		 */
		void update(VkDescriptorSet set);

		/**
		 * Bind a buffer to the given location.
		 *
		 * @param location The location to bind to.
		 * @param buffer The buffer to bind.
		 */
		void bindResource(uint32_t location, const Buffer& buffer);

		/**
		 * Bind a image to the given location.
		 *
		 * @param location The location to bind to.
		 * @param image The image to bind.
		 */
		void bindResource(uint32_t location, const Image& image);

		/**
		 * Get the descriptor set.
		 *
		 * @return The descriptor set.
		 */
		VkDescriptorSet getDescriptorSet() const { return m_DescriptorSet; }

	private:
		std::set<uint32_t> m_Bindings;

		GraphicsEngine& m_Engine;

		const VkDescriptorSetLayout m_vDescriptorSetLayout;
		VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
	};
}