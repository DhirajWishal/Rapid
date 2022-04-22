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

		// Make sure to show the window!
		SDL_ShowWindow(m_pWindow);

		// Create the surface.
		if (!SDL_Vulkan_CreateSurface(m_pWindow, m_Engine.getInstance(), &m_Surface))
		{
			spdlog::error("Failed to create the window surface! Error message: {}", SDL_GetError());
			return;
		}

		// Get the frame count.
		m_FrameCount = getBestBufferCount();

		// Create the swapchain and the rest of rendering components.
		createSwapchain();
		createRenderPass();
		createFramebuffers();
		createSyncObjects();

		// Create the command buffer allocator.
		m_CommandBufferAllocator = std::make_unique<CommandBufferAllocator>(m_Engine, m_FrameCount);
	}

	Window::~Window()
	{
		if (isActive())
			terminate();
	}

	void Window::terminate()
	{
		m_ProcessingNodes.clear();

		m_CommandBufferAllocator->terminate();
		m_Engine.getDeviceTable().vkDestroyRenderPass(m_Engine.getLogicalDevice(), m_RenderPass, nullptr);

		for (auto vFrameBuffer : m_Framebuffers)
			m_Engine.getDeviceTable().vkDestroyFramebuffer(m_Engine.getLogicalDevice(), vFrameBuffer, nullptr);

		for (auto vSemaphore : m_RenderFinishedSemaphores)
			m_Engine.getDeviceTable().vkDestroySemaphore(m_Engine.getLogicalDevice(), vSemaphore, nullptr);

		for (auto vSemaphore : m_InFlightSemaphores)
			m_Engine.getDeviceTable().vkDestroySemaphore(m_Engine.getLogicalDevice(), vSemaphore, nullptr);

		clearSwapchain();
		vkDestroySurfaceKHR(m_Engine.getInstance(), m_Surface, nullptr);
		m_IsTerminated = true;
	}

	void Window::pollEvents()
	{
		SDL_Event sdlEvent = {};
		const auto isAvailable = SDL_PollEvent(&sdlEvent);

		// Close the application. This is a naive way of doing it and will be replaced later.
		if (sdlEvent.type == SDL_QUIT)
			std::exit(0);

		// Transmit the data to the nodes.
		for (auto& pNode : m_ProcessingNodes)
			pNode->onPollEvents(sdlEvent);

		// Acquire the next swapchain image.
		const auto result = m_Engine.getDeviceTable().vkAcquireNextImageKHR(m_Engine.getLogicalDevice(), m_Swapchain, std::numeric_limits<uint64_t>::max(), m_InFlightSemaphores[m_FrameIndex], VK_NULL_HANDLE, &m_ImageIndex);
		if (result == VkResult::VK_ERROR_OUT_OF_DATE_KHR || result == VkResult::VK_SUBOPTIMAL_KHR)
		{
			// Recreate.
		}
		else
			utility::ValidateResult(result, "Failed to acquire the next swap chain image!");
	}

	void Window::submitFrame()
	{
		auto commandBuffer = m_CommandBufferAllocator->getCommandBuffer(m_FrameIndex);
		commandBuffer.begin();

		// Set the clear value.
		VkClearValue clearValue = {
			.color = {
				.float32 = {0.0f, 0.0f, 0.0f, 1.0f}
			}
		};

		// Bind the render pass.
		commandBuffer.bindWindow(*this, { clearValue });

		// Bind all the nodes.
		for (auto& pNode : m_ProcessingNodes)
			pNode->bind(commandBuffer, m_FrameIndex);

		// End the render pass and command buffer.
		commandBuffer.unbindWindow();
		commandBuffer.end();

		// Submit the commands.
		commandBuffer.submit(m_RenderFinishedSemaphores[m_FrameIndex], m_InFlightSemaphores[m_FrameIndex], true);	// Remove the true here later.

		// We can now present it.
		present();

		// Finally, increment the frame index.
		m_FrameIndex = ++m_FrameIndex % m_FrameCount;
	}

	VkExtent2D Window::extent() const
	{
		int32_t width = 0, height = 0;
		SDL_GetWindowSize(m_pWindow, &width, &height);

		return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
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

		// Get the extent.
		const auto imageExtent = extent();

		// Create the swap chain.
		VkSwapchainCreateInfoKHR swapchainCreateInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.pNext = VK_NULL_HANDLE,
			.flags = 0,
			.surface = m_Surface,
			.minImageCount = m_FrameCount,
			.imageFormat = m_SwapchainFormat,
			.imageColorSpace = surfaceFormat.colorSpace,
			.imageExtent = imageExtent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
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

	void Window::createRenderPass()
	{
		// Crate attachment descriptions.
		VkAttachmentDescription attachmentDescription = {
			.flags = 0,
			.format = m_SwapchainFormat,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		};

		// Create the subpass dependencies.
		std::array<VkSubpassDependency, 2> subpassDependencies;
		subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		subpassDependencies[1].srcSubpass = 0;
		subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		// Create the subpass description.
		VkAttachmentReference colorAttachmentReference = {
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription subpassDescription = {
			.flags = 0,
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachmentReference,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = nullptr,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
		};

		// Create the render target.
		VkRenderPassCreateInfo renderPassCreateInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = 1,
			.pAttachments = &attachmentDescription,
			.subpassCount = 1,
			.pSubpasses = &subpassDescription,
			.dependencyCount = 2,
			.pDependencies = subpassDependencies.data(),
		};

		utility::ValidateResult(m_Engine.getDeviceTable().vkCreateRenderPass(m_Engine.getLogicalDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass), "Failed to create render pass!");
	}

	void Window::createFramebuffers()
	{
		const auto imageExtent = extent();

		VkFramebufferCreateInfo frameBufferCreateInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0,
			.renderPass = m_RenderPass,
			.attachmentCount = 1,
			.width = imageExtent.width,
			.height = imageExtent.height,
			.layers = 1,
		};

		// Iterate and create the frame buffers.
		m_Framebuffers.resize(m_FrameCount);
		for (uint8_t i = 0; i < m_FrameCount; i++)
		{
			frameBufferCreateInfo.pAttachments = &m_SwapchainImageViews[i];
			utility::ValidateResult(m_Engine.getDeviceTable().vkCreateFramebuffer(m_Engine.getLogicalDevice(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]), "Failed to create the frame buffer!");
		}
	}

	void Window::createSyncObjects()
	{
		VkSemaphoreCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0
		};

		m_RenderFinishedSemaphores.reserve(m_FrameCount);
		m_InFlightSemaphores.reserve(m_FrameCount);
		for (uint8_t i = 0; i < m_FrameCount; i++)
		{
			VkSemaphore vRenderFinishedSemaphore = VK_NULL_HANDLE;
			utility::ValidateResult(m_Engine.getDeviceTable().vkCreateSemaphore(m_Engine.getLogicalDevice(), &createInfo, nullptr, &vRenderFinishedSemaphore), "Failed to create the frame buffer!");
			m_RenderFinishedSemaphores.emplace_back(vRenderFinishedSemaphore);

			VkSemaphore vInFlightSemaphore = VK_NULL_HANDLE;
			utility::ValidateResult(m_Engine.getDeviceTable().vkCreateSemaphore(m_Engine.getLogicalDevice(), &createInfo, nullptr, &vInFlightSemaphore), "Failed to create the frame buffer!");
			m_InFlightSemaphores.emplace_back(vInFlightSemaphore);
		}
	}

	void Window::present()
	{
		VkPresentInfoKHR presentInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext = nullptr,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &m_RenderFinishedSemaphores[m_ImageIndex],
			.swapchainCount = 1,
			.pSwapchains = &m_Swapchain,
			.pImageIndices = &m_ImageIndex,
			.pResults = VK_NULL_HANDLE,
		};

		const auto result = m_Engine.getDeviceTable().vkQueuePresentKHR(m_Engine.getQueue().getTransferQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			// Recreate.
		}
		else
			utility::ValidateResult(result, "Failed to present the swapchain image!");
	}
}