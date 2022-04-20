// Copyright (c) 2022 Dhiraj Wishal

#include "Utility.hpp"

#include <spdlog/spdlog.h>

namespace rapid
{
	namespace utility
	{
		void ValidateResult(VkResult result, std::string_view message)
		{
			if (result != VK_SUCCESS)
				spdlog::error(message);
		}
	}
}