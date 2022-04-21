// Copyright (c) 2022 Dhiraj Wishal

#include "Window.hpp"
#include "GraphicsEngine.hpp"
#include "Utility.hpp"

#include <spdlog/spdlog.h>
#include <SDL_vulkan.h>

namespace rapid
{
	Window::Window(GraphicsEngine& engine, std::string_view title)
		: m_Engine(engine), m_pWindow(SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOWEVENT_MAXIMIZED))
	{
		// Check if the window creation was successful.
		if (!m_pWindow)
		{
			spdlog::error("Failed to create the window! Error message: {}", SDL_GetError());
			return;
		}

		// Create the surface.
		if (!SDL_Vulkan_CreateSurface(m_pWindow, m_Engine.getInstance(), &m_Surface))
		{
			spdlog::error("Failed to create the window surface! Error message: {}", SDL_GetError());
			return;
		}

		// Create the swapchain.
		createSwapchain();
	}

	Window::~Window()
	{
		if (isActive())
			terminate();
	}

	void Window::terminate()
	{
		clearSwapchain();
		vkDestroySurfaceKHR(m_Engine.getInstance(), m_Surface, nullptr);
		m_IsTerminated = true;
	}

	VkExtent2D Window::extent() const
	{
		int32_t width = 0, height = 0;
		SDL_GetWindowSize(m_pWindow, &width, &height);

		return VkExtent2D(width, height);
	}

	uint32_t Window::getBestBufferCount() const
	{
		// Get the surface capabilities.
		VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
		utility::ValidateResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Engine.getPhysicalDevice(), m_Surface, &surfaceCapabilities), "Failed to get the surface capabilities!");

		// Resolve the best buffer count.
		uint32_t bufferCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount > 0 && bufferCount > surfaceCapabilities.maxImageCount)
			bufferCount = surfaceCapabilities.maxImageCount;

		return bufferCount;
	}

	void Window::clearSwapchain()
	{
		// Terminate the image views.
		for (auto view : m_SwapchainImageViews)
			m_Engine.getDeviceTable().vkDestroyImageView(m_Engine.getLogicalDevice(), view, nullptr);

		// Now we can destroy the swapchain.
		m_Engine.getDeviceTable().vkDestroySwapchainKHR(m_Engine.getLogicalDevice(), m_Swapchain, nullptr);

		// Clean the variables.
		m_Swapchain = VK_NULL_HANDLE;
		m_SwapchainImages.clear();
		m_SwapchainImageViews.clear();
	}

	void Window::resolveImageViews()
	{
		VkImageViewCreateInfo viewCreateInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = m_SwapchainFormat,
			.components = {},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};

		m_SwapchainImageViews.resize(m_SwapchainImages.size());

		VkImageView* pArray = m_SwapchainImageViews.data();
		for (auto itr = m_SwapchainImages.begin(); itr != m_SwapchainImages.end(); itr++)
		{
			viewCreateInfo.image = *itr;
			utility::ValidateResult(m_Engine.getDeviceTable().vkCreateImageView(m_Engine.getLogicalDevice(), &viewCreateInfo, nullptr, pArray), "Failed to create the swapchain image view!");
			pArray++;
		}
	}

	void Window::createSwapchain()
	{
		// Get the surface capabilities.
		VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
		utility::ValidateResult(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Engine.getPhysicalDevice(), m_Surface, &surfaceCapabilities), "Failed to get the surface capabilities!");

		// Get the surface formats.
		uint32_t formatCount = 0;
		utility::ValidateResult(vkGetPhysicalDeviceSurfaceFormatsKHR(m_Engine.getPhysicalDevice(), m_Surface, &formatCount, nullptr), "Failed to get the surface format count!");

		if (formatCount == 0)
		{
			spdlog::error("No suitable surface formats found!");
			return;
		}

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		utility::ValidateResult(vkGetPhysicalDeviceSurfaceFormatsKHR(m_Engine.getPhysicalDevice(), m_Surface, &formatCount, surfaceFormats.data()), "Failed to get the surface formats!");

		// Get the present modes.
		uint32_t presentModeCount = 0;
		utility::ValidateResult(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Engine.getPhysicalDevice(), m_Surface, &presentModeCount, nullptr), "Failed to get the surface present mode count!");

		if (presentModeCount == 0)
		{
			spdlog::error("No suitable present formats found!");
			return;
		}

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		utility::ValidateResult(vkGetPhysicalDeviceSurfacePresentModesKHR(m_Engine.getPhysicalDevice(), m_Surface, &presentModeCount, presentModes.data()), "Failed to get the surface present modes!");

		// Check if we have the present mode we need.
		bool bPresentModeAvailable = false;
		VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
		for (const auto availablePresentMode : presentModes)
		{
			if (availablePresentMode == presentMode)
			{
				bPresentModeAvailable = true;
				break;
			}
		}

		// If not available, let's just use the first one we got.
		if (!bPresentModeAvailable)
			presentMode = presentModes.front();

		// Resolve the surface composite.
		VkCompositeAlphaFlagBitsKHR surfaceComposite = static_cast<VkCompositeAlphaFlagBitsKHR>(surfaceCapabilities.supportedCompositeAlpha);
		surfaceComposite = (surfaceComposite & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
			? VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
			: (surfaceComposite & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
			? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR
			: (surfaceComposite & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
			? VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
			: VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

		// Get the best surface format.
		VkSurfaceFormatKHR surfaceFormat = surfaceFormats.front();
		for (const auto& availableFormat : surfaceFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				surfaceFormat = availableFormat;
		}

		m_SwapchainFormat = surfaceFormat.format;

		// Get the best image count and the extent.
		const auto imageCount = getBestBufferCount();
		const auto imageExtent = extent();

		// Create the swap chain.
		VkSwapchainCreateInfoKHR swapchainCreateInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = VK_NULL_HANDLE,
			.flags = 0,
			.surface = m_Surface,
			.minImageCount = imageCount,
			.imageFormat = m_SwapchainFormat,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = imageExtent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr,
			.preTransform = surfaceCapabilities.currentTransform,
			.compositeAlpha = surfaceComposite,
			.presentMode = presentMode,
			.clipped = VK_TRUE,
			.oldSwapchain = m_Swapchain,
		};

		// Resolve the queue families if the two queues are different.
		uint32_t queueFamilyindices[2] = {
				m_Engine.getQueue().getGraphicsFamily().value(),
				m_Engine.getQueue().getTransferFamily().value()
		};

		if (m_Engine.getQueue().getGraphicsFamily() != m_Engine.getQueue().getTransferFamily())
		{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyindices;
		}

		VkSwapchainKHR vNewSwapChain = VK_NULL_HANDLE;
		utility::ValidateResult(m_Engine.getDeviceTable().vkCreateSwapchainKHR(m_Engine.getLogicalDevice(), &swapchainCreateInfo, nullptr, &vNewSwapChain), "Failed to create the swapchain!");

		// Destroy the old swapchain if we had it.
		if (m_Swapchain != VK_NULL_HANDLE)
			clearSwapchain();

		// Assign the new swapchain.
		m_Swapchain = vNewSwapChain;

		// Get the image views.
		m_SwapchainImages.resize(swapchainCreateInfo.minImageCount);
		utility::ValidateResult(m_Engine.getDeviceTable().vkGetSwapchainImagesKHR(m_Engine.getLogicalDevice(), m_Swapchain, &swapchainCreateInfo.minImageCount, m_SwapchainImages.data()), "Failed to get the swapchain images!");

		// Finally we can resolve the swapchain image views.
		resolveImageViews();
	}
}