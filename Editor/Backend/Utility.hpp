// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <vulkan/vulkan.hpp>
#include <string>

namespace rapid
{
	namespace utility
	{
		/**
		 * Validate the result from a Vulkan function.
		 *
		 * @param result The result returned by the function.
		 */
		void ValidateResult(VkResult result, std::string_view message);
	}
}