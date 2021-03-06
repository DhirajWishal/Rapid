// Copyright (c) 2022 Dhiraj Wishal

#define VMA_IMPLEMENTATION
#include "GraphicsEngine.hpp"
#include "Utility.hpp"
#include "Image.hpp"

#include <SDL_vulkan.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <array>
#include <set>
#include <filesystem>

namespace
{
	/**
	 * Create a color from the float value from 0 - 255.
	 *
	 * @param value The color value.
	 * @return The created color value.
	 */
	constexpr float CreateColor256(float value) { return value / 256; }

	/**
	 * Get the required extension names.
	 *
	 * @return The extension names.
	 */
	std::vector<const char*> GetRequiredExtensions()
	{
		uint32_t extensionCount = 0;
		rapid::utility::ValidateResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr), "Failed to enumerate instance extension property count!");

		std::vector<VkExtensionProperties> extensions(extensionCount);
		rapid::utility::ValidateResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()), "Failed to enumerate instance extension property count!");

		std::vector<const char*> extensionNames;
		extensionNames.reserve(extensionCount);

		// We just need the extension names so we can put those to the vector.
		for (const auto& extension : extensions)
			extensionNames.emplace_back(extension.extensionName);

		return extensionNames;
	}

#ifdef RAPID_DEBUG
	/**
	 * Vulkan debug callback.
	 * This function will be called when a debug message is to be printed by the Vulkan framework.
	 *
	 * @param messageSeverity The severity of the message.
	 * @param messageType The type of the message.
	 * @param pCallbackData The callback data. This may contain valuable information regarding an error.
	 * @param pUseData The user data that was provided at the time of the error.
	 * @return A boolean value.
	 */
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::string myMessagePreStatement = ": ";
		if (messageType & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
			myMessagePreStatement += "GENERAL | ";
		else if (messageType & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			myMessagePreStatement += "VALIDATION | ";
		else if (messageType & VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			myMessagePreStatement += "PERFORMANCE | ";

		std::stringstream messageStream;
		messageStream << "Vulkan Validation Layer " << myMessagePreStatement << pCallbackData->pMessage;

		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			spdlog::warn(messageStream.str());
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			spdlog::error(messageStream.str());
			break;

		default:
			spdlog::info(messageStream.str());
			break;
		}

		return VK_FALSE;
	}

	/**
	 * Create the debug messenger create info.
	 *
	 * @return The create info structure.
	 */
	VkDebugUtilsMessengerCreateInfoEXT CreateDebugMessengerCreateInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT vCreateInfo = {};
		vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		vCreateInfo.pNext = VK_NULL_HANDLE;
		vCreateInfo.pUserData = VK_NULL_HANDLE;
		vCreateInfo.flags = 0;
		vCreateInfo.pfnUserCallback = DebugCallback;

		vCreateInfo.messageSeverity
			= VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

		vCreateInfo.messageType
			= VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		return vCreateInfo;
	}

#endif

	/**
	 * Check device extension support.
	 *
	 * @param vPhysicalDevice The physical device to check.
	 * @param deviceExtensions The extension to check.
	 */
	bool CheckDeviceExtensionSupport(VkPhysicalDevice vPhysicalDevice, const std::vector<const char*>& deviceExtensions)
	{
		// If there are no extension to check, we can just return true.
		if (deviceExtensions.empty())
			return true;

		// Get the extension count.
		uint32_t extensionCount = 0;
		rapid::utility::ValidateResult(vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, nullptr), "Failed to enumerate physical device extension property count!");

		// Load the extensions.
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		rapid::utility::ValidateResult(vkEnumerateDeviceExtensionProperties(vPhysicalDevice, nullptr, &extensionCount, availableExtensions.data()), "Failed to enumerate physical device extension properties!");

		std::set<std::string_view> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// Iterate and check if it contains the extensions we need. If it does, remove them from the set so we can later check if 
		// all the required extensions exist.
		for (const VkExtensionProperties& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		// If the required extensions set is empty, it means that all the required extensions exist within the physical device.
		return requiredExtensions.empty();
	}

	/**
	 * Check if a physical device is suitable.
	 *
	 * @param vPhysicalDevice The physical device to check.
	 * @param deviceExtensions The device extension to check.
	 */
	bool IsPhysicalDeviceSuitable(VkPhysicalDevice vPhysicalDevice, const std::vector<const char*>& deviceExtensions)
	{
		// Check if all the provided queue flags are supported.
		const auto queue = rapid::Queue(vPhysicalDevice);

		return CheckDeviceExtensionSupport(vPhysicalDevice, deviceExtensions) && queue.isComplete();
	}

	/**
	 * Static initializer class.
	 * This class is initialized as a static variable and contains the initialization functions required to be run
	 * only once.
	 */
	struct StaticInitializer
	{
		/**
		 * Default constructor.
		 */
		StaticInitializer()
		{
			// First, initialize volk. Without this we can't do anything else.
			rapid::utility::ValidateResult(volkInitialize(), "Failed to initialize volk!");

			// Initialize SDL and create the main window.
			SDL_Init(SDL_INIT_VIDEO);

			// Initialize ImGui.
			ImGui::CreateContext();
			setupImGui();
		}

		/**
		 * Default destructor.
		 */
		~StaticInitializer()
		{
			ImGui::DestroyContext();
			SDL_Quit();
		}

	private:
		/**
		 * Setup ImGui.
		 */
		void setupImGui() const
		{
			auto& style = ImGui::GetStyle();

			// Background - 20, 23, 27
			// Tabs - 242, 84, 91
			// Menus - 25, 133, 161

			style.Colors[ImGuiCol_TitleBg] = ImVec4(CreateColor256(26), CreateColor256(30), CreateColor256(35), 0.5f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(CreateColor256(26), CreateColor256(30), CreateColor256(35), 0.75f);

			style.Colors[ImGuiCol_WindowBg] = ImVec4(CreateColor256(26), CreateColor256(30), CreateColor256(35), 1.0f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(CreateColor256(26), CreateColor256(30), CreateColor256(35), 1.0f);

			style.Colors[ImGuiCol_Header] = ImVec4(CreateColor256(25), CreateColor256(133), CreateColor256(161), 0.5f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(CreateColor256(25), CreateColor256(133), CreateColor256(161), 1.0f);

			style.Colors[ImGuiCol_Tab] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 0.25f);
			style.Colors[ImGuiCol_TabActive] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 0.75f);
			style.Colors[ImGuiCol_TabHovered] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 1.0f);
			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 0.5f);
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4(CreateColor256(242), CreateColor256(84), CreateColor256(91), 0.25f);

			style.ChildRounding = 6.0f;
			style.FrameRounding = 1.0f;
			style.FramePadding.x = 5.0f;
			style.FramePadding.y = 2.0f;
			style.PopupRounding = 3.0f;
			style.TabRounding = 1.0f;
			style.WindowRounding = 3.0f;
			//style.WindowPadding.x = 5.0f;

			ImGuiIO& imGuiIO = ImGui::GetIO();
			imGuiIO.Fonts->AddFontFromFileTTF((std::filesystem::current_path() / "Fonts" / "Manrope" / "static" / "Manrope-Regular.ttf").string().c_str(), 16.0f);

			//imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
			imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		 // Enable Keyboard Controls
			imGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			imGuiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
			imGuiIO.MouseDrawCursor = true;
		}
	};
}

namespace rapid
{
	GraphicsEngine::GraphicsEngine()
	{
		// Set up the static initializer.
		static StaticInitializer initializer;

		// Initialize the instance and the rest.
		createInstance();
		selectPhysicalDevice();
		createLogicalDevice();
		createUtilityCommandBuffer();
	}

	GraphicsEngine::~GraphicsEngine()
	{
		if (isActive())
			terminate();
	}

	void GraphicsEngine::terminate()
	{
		m_DeviceTable.vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &m_CommandBuffer);
		m_DeviceTable.vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);

		vmaDestroyAllocator(m_vAllocator);
		vkDestroyDevice(m_LogicalDevice, nullptr);

#ifdef RAPID_DEBUG
		const auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
		vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

#endif // RAPID_DEBUG

		vkDestroyInstance(m_Instance, nullptr);

		m_IsTerminated = true;
	}

	VkCommandBuffer GraphicsEngine::beginCommandBufferRecording()
	{
		// Skip if we're on the recording state.
		if (m_IsRecording)
			return m_CommandBuffer;

		// Begin recording.
		VkCommandBufferBeginInfo beginInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = nullptr,
			.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		};

		utility::ValidateResult(m_DeviceTable.vkBeginCommandBuffer(m_CommandBuffer, &beginInfo), "Failed to begin command buffer recording!");

		m_IsRecording = true;
		return m_CommandBuffer;
	}

	void GraphicsEngine::endCommandBufferRecording()
	{
		// Skip if we weren't recording.
		if (!m_IsRecording)
			return;

		utility::ValidateResult(m_DeviceTable.vkEndCommandBuffer(m_CommandBuffer), "Failed to end command buffer recording!");
		m_IsRecording = false;
	}

	void GraphicsEngine::executeRecordedCommands(bool shouldWait)
	{
		// End recording if we haven't.
		endCommandBufferRecording();

		VkSubmitInfo submitInfo = {
			.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &m_CommandBuffer
		};

		VkFence fence = VK_NULL_HANDLE;

		// Create the fence if we need to wait.
		if (shouldWait)
		{
			VkFenceCreateInfo vFenceCreateInfo = {
				.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.pNext = VK_NULL_HANDLE,
				.flags = 0
			};

			utility::ValidateResult(m_DeviceTable.vkCreateFence(getLogicalDevice(), &vFenceCreateInfo, nullptr, &fence), "Failed to create the synchronization fence!");
		}

		// Submit the queue.
		utility::ValidateResult(m_DeviceTable.vkQueueSubmit(m_Queue.getTransferQueue(), 1, &submitInfo, fence), "Failed to submit the queue!");

		// Destroy the fence if we created it.
		if (shouldWait)
		{
			utility::ValidateResult(m_DeviceTable.vkWaitForFences(getLogicalDevice(), 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()), "Failed to wait for the fence!");
			m_DeviceTable.vkDestroyFence(getLogicalDevice(), fence, nullptr);
		}
	}

	void GraphicsEngine::waitIdle() const
	{
		m_DeviceTable.vkDeviceWaitIdle(m_LogicalDevice);
	}

	void GraphicsEngine::createInstance()
	{
		VkApplicationInfo applicationInfo =
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "Rapid",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = volkGetInstanceVersion()
		};

		// Setup the extensions.
		uint32_t extensionCount = 0;
		utility::ValidateResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr), "Failed to enumerate instance extension property count!");

		std::vector<VkExtensionProperties> extensions(extensionCount);
		utility::ValidateResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()), "Failed to enumerate instance extension property count!");

		std::vector<const char*> extensionNames;
		extensionNames.reserve(extensionCount);

		// We just need the extension names so we can put those to the vector.
		for (const auto& extension : extensions)
			extensionNames.emplace_back(extension.extensionName);

		VkInstanceCreateInfo instanceCreateInfo =
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size()),
			.ppEnabledLayerNames = m_ValidationLayers.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
			.ppEnabledExtensionNames = extensionNames.data()
		};

#ifdef RAPID_DEBUG
		m_ValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
		const auto debugMessengerCreateInfo = CreateDebugMessengerCreateInfo();

		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();
		instanceCreateInfo.pNext = &debugMessengerCreateInfo;

#endif // RAPID_DEBUG

		// Create the instance.
		utility::ValidateResult(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance), "Failed to create the Vulkan instance!");

#ifdef RAPID_DEBUG
		const auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
		utility::ValidateResult(vkCreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger), "Failed to create the debug messenger.");

#endif // RAPID_DEBUG

		// Load the instance functions.
		volkLoadInstance(m_Instance);
	}

	void GraphicsEngine::selectPhysicalDevice()
	{
		// Set up the device extensions.
		m_DeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		// Enumerate physical devices.
		uint32_t deviceCount = 0;
		utility::ValidateResult(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr), "Failed to enumerate physical devices.");

		// Throw an error if there are no physical devices available.
		if (deviceCount == 0)
		{
			spdlog::error("No physical devices found!");
			return;
		}

		std::vector<VkPhysicalDevice> vCandidates(deviceCount);
		utility::ValidateResult(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, vCandidates.data()), "Failed to enumerate physical devices.");

		struct Candidate { VkPhysicalDeviceProperties m_Properties; VkPhysicalDevice m_Candidate; };
		std::array<Candidate, 6> vPriorityMap = { Candidate() };

		// Iterate through all the candidate devices and find the best device.
		for (const auto& vCandidateDevice : vCandidates)
		{
			// Check if the device is suitable for our use.
			if (IsPhysicalDeviceSuitable(vCandidateDevice, m_DeviceExtensions))
			{
				VkPhysicalDeviceProperties vPhysicalDeviceProperties = {};
				vkGetPhysicalDeviceProperties(vCandidateDevice, &vPhysicalDeviceProperties);

				// Sort the candidates by priority.
				uint8_t priorityIndex = 5;
				switch (vPhysicalDeviceProperties.deviceType)
				{
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					priorityIndex = 0;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					priorityIndex = 1;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
					priorityIndex = 2;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_CPU:
					priorityIndex = 3;
					break;

				case VK_PHYSICAL_DEVICE_TYPE_OTHER:
					priorityIndex = 4;
					break;

				default:
					priorityIndex = 5;
					break;
				}

				vPriorityMap[priorityIndex].m_Candidate = vCandidateDevice;
				vPriorityMap[priorityIndex].m_Properties = vPhysicalDeviceProperties;
			}
		}

		// Choose the physical device with the highest priority.
		for (const auto& candidate : vPriorityMap)
		{
			if (candidate.m_Candidate != VK_NULL_HANDLE)
			{
				m_PhysicalDevice = candidate.m_Candidate;
				m_Properties = candidate.m_Properties;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			spdlog::error("Failed to find a suitable physical device!");
			return;
		}

		// Create the queue.
		m_Queue = Queue(m_PhysicalDevice);
	}

	VmaVulkanFunctions GraphicsEngine::getVmaFunctions() const
	{
		VmaVulkanFunctions functions = {
			.vkGetInstanceProcAddr = vkGetInstanceProcAddr,
			.vkGetDeviceProcAddr = vkGetDeviceProcAddr,
			.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
			.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
			.vkAllocateMemory = m_DeviceTable.vkAllocateMemory,
			.vkFreeMemory = m_DeviceTable.vkFreeMemory,
			.vkMapMemory = m_DeviceTable.vkMapMemory,
			.vkUnmapMemory = m_DeviceTable.vkUnmapMemory,
			.vkFlushMappedMemoryRanges = m_DeviceTable.vkFlushMappedMemoryRanges,
			.vkInvalidateMappedMemoryRanges = m_DeviceTable.vkInvalidateMappedMemoryRanges,
			.vkBindBufferMemory = m_DeviceTable.vkBindBufferMemory,
			.vkBindImageMemory = m_DeviceTable.vkBindImageMemory,
			.vkGetBufferMemoryRequirements = m_DeviceTable.vkGetBufferMemoryRequirements,
			.vkGetImageMemoryRequirements = m_DeviceTable.vkGetImageMemoryRequirements,
			.vkCreateBuffer = m_DeviceTable.vkCreateBuffer,
			.vkDestroyBuffer = m_DeviceTable.vkDestroyBuffer,
			.vkCreateImage = m_DeviceTable.vkCreateImage,
			.vkDestroyImage = m_DeviceTable.vkDestroyImage,
			.vkCmdCopyBuffer = m_DeviceTable.vkCmdCopyBuffer,
			.vkGetBufferMemoryRequirements2KHR = m_DeviceTable.vkGetBufferMemoryRequirements2KHR,
			.vkGetImageMemoryRequirements2KHR = m_DeviceTable.vkGetImageMemoryRequirements2KHR,
			.vkBindBufferMemory2KHR = m_DeviceTable.vkBindBufferMemory2KHR,
			.vkBindImageMemory2KHR = m_DeviceTable.vkBindImageMemory2KHR,
			.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR,
			.vkGetDeviceBufferMemoryRequirements = m_DeviceTable.vkGetDeviceBufferMemoryRequirements,
			.vkGetDeviceImageMemoryRequirements = m_DeviceTable.vkGetDeviceImageMemoryRequirements
		};

		return functions;
	}

	void GraphicsEngine::createLogicalDevice()
	{
		// Setup device queues.
		constexpr float priority = 1.0f;
		std::set<uint32_t> uniqueQueueFamilies = {
			m_Queue.getTransferFamily().value(),
			m_Queue.getGraphicsFamily().value()
		};

		VkDeviceQueueCreateInfo queueCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = 0,
			.queueCount = 1,
			.pQueuePriorities = &priority,
		};

		std::vector< VkDeviceQueueCreateInfo> queueCreateInfos;
		for (const auto& family : uniqueQueueFamilies)
		{
			queueCreateInfo.queueFamilyIndex = family;
			queueCreateInfos.emplace_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures features = {};
		features.samplerAnisotropy = VK_TRUE;
		features.sampleRateShading = VK_TRUE;
		features.tessellationShader = VK_TRUE;
		features.geometryShader = VK_TRUE;

		// Device create info.
		VkDeviceCreateInfo deviceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size()),
			.ppEnabledExtensionNames = m_DeviceExtensions.data(),
			.pEnabledFeatures = &features
		};

#ifdef RAPID_DEBUG
		// Get the validation layers and initialize it.
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = m_ValidationLayers.data();

#endif // RAPID_DEBUG

		// Create the device.
		utility::ValidateResult(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_LogicalDevice), "Failed to create the logical device!");

		// Load the device table.
		volkLoadDeviceTable(&m_DeviceTable, m_LogicalDevice);

		// Get the queues.
		vkGetDeviceQueue(m_LogicalDevice, m_Queue.getTransferFamily().value(), 0, &m_Queue.getTransferQueue());
		vkGetDeviceQueue(m_LogicalDevice, m_Queue.getGraphicsFamily().value(), 0, &m_Queue.getGraphicsQueue());

		// Create VMA allocator.
		const auto functions = getVmaFunctions();
		VmaAllocatorCreateInfo vmaCreateInfo = {
			.flags = 0,
			.physicalDevice = m_PhysicalDevice,
			.device = m_LogicalDevice,
			.pVulkanFunctions = &functions,
			.instance = m_Instance,
			.vulkanApiVersion = volkGetInstanceVersion()
		};

		utility::ValidateResult(vmaCreateAllocator(&vmaCreateInfo, &m_vAllocator), "Failed to create the allocator!");
	}

	void GraphicsEngine::createUtilityCommandBuffer()
	{
		// Create the command pool.
		VkCommandPoolCreateInfo commandPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = m_Queue.getTransferFamily().value()
		};

		utility::ValidateResult(m_DeviceTable.vkCreateCommandPool(m_LogicalDevice, &commandPoolCreateInfo, nullptr, &m_CommandPool), "Failed to create the command pool!");

		// Allocate the command buffer.
		VkCommandBufferAllocateInfo allocateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.commandPool = m_CommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		utility::ValidateResult(m_DeviceTable.vkAllocateCommandBuffers(m_LogicalDevice, &allocateInfo, &m_CommandBuffer), "Failed to allocate command buffer!");
	}
}