// Copyright (c) 2022 Dhiraj Wishal

#define VMA_IMPLEMENTATION
#include "GraphicsEngine.hpp"
#include "Utility.hpp"

#include <SDL_vulkan.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <array>
#include <set>

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
}

namespace rapid
{
	GraphicsEngine::GraphicsEngine()
	{
		// FIrst of all, initialize volk. Without this we can't do anything else.
		utility::ValidateResult(volkInitialize(), "Failed to initialize volk!");

		// Initialize SDL and create the main window.
		SDL_Init(SDL_INIT_VIDEO);
		m_Windows.emplace_back(Window("Rapid Editor"));

		// Initialize the instance and the rest.
		createInstance();
		selectPhysicalDevice();
		createLogicalDevice();

		// Make sure to set up ImGui.
		setupImGui();
	}

	GraphicsEngine::~GraphicsEngine()
	{
		if (!m_IsTerminated)
			terminate();
	}

	void GraphicsEngine::terminate()
	{
		ImGui::DestroyContext();

		m_Windows.clear();
		SDL_Quit();

		vkDestroyDevice(m_LogicalDevice, nullptr);

#ifdef RAPID_DEBUG
		const auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
		vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

#endif // RAPID_DEBUG

		vkDestroyInstance(m_Instance, nullptr);

		m_IsTerminated = true;
	}

	void GraphicsEngine::beginFrame()
	{
		ImGui::NewFrame();

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace", NULL,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground
		);

		ImGui::PopStyleVar(3);
		ImGui::DockSpace(ImGui::GetID("EditorDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode);
	}

	void GraphicsEngine::endFrame()
	{
		ImGui::Render();
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
		rapid::utility::ValidateResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr), "Failed to enumerate instance extension property count!");

		std::vector<VkExtensionProperties> extensions(extensionCount);
		rapid::utility::ValidateResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()), "Failed to enumerate instance extension property count!");

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
		std::array<Candidate, 6> vPriorityMap;

		// Iterate through all the candidate devices and find the best device.
		for (const auto& vCandidateDevice : vCandidates)
		{
			// Check if the device is suitable for our use.
			if (IsPhysicalDeviceSuitable(vCandidateDevice, {}))
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

	void GraphicsEngine::createLogicalDevice()
	{
		// Setup device queues.
		constexpr float priority = 1.0f;
		std::set<uint32_t> uniqueQueueFamilies = {
			m_Queue.getTransferFamily().value(),
			m_Queue.getGraphicsFamily().value()
		};

		std::vector< VkDeviceQueueCreateInfo> queueCreateInfos;

		VkDeviceQueueCreateInfo queueCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueFamilyIndex = 0,
			.queueCount = 1,
			.pQueuePriorities = &priority,
		};

		for (const auto& family : uniqueQueueFamilies)
		{
			queueCreateInfo.queueFamilyIndex = family;
			queueCreateInfos.emplace_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures features = {};
		//features.samplerAnisotropy = VK_TRUE;
		//features.sampleRateShading = VK_TRUE;
		//features.tessellationShader = VK_TRUE;

		// Device create info.
		VkDeviceCreateInfo deviceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = nullptr,
			.enabledExtensionCount = 0,
			.ppEnabledExtensionNames = nullptr,
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
	}

	void GraphicsEngine::setupImGui() const
	{
		ImGui::CreateContext();
		ImGuiStyle& style = ImGui::GetStyle();

		const auto backgroundColor = ImVec4(CreateColor256(34), CreateColor256(40), CreateColor256(49), 1.0f);
		style.Colors[ImGuiCol_TitleBg] = backgroundColor;
		style.Colors[ImGuiCol_WindowBg] = backgroundColor;
		style.Colors[ImGuiCol_MenuBarBg] = backgroundColor;

		const auto mainColor = ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = mainColor;
		style.Colors[ImGuiCol_Header] = mainColor;
		style.Colors[ImGuiCol_TabActive] = mainColor;
		style.Colors[ImGuiCol_TabUnfocusedActive] = mainColor;

		const auto hoveredColor = ImVec4(CreateColor256(0), CreateColor256(173), CreateColor256(181), 1.0f);
		style.Colors[ImGuiCol_TabHovered] = hoveredColor;
		style.Colors[ImGuiCol_HeaderHovered] = hoveredColor;

		style.ChildRounding = 6.0f;
		style.FrameRounding = 3.0f;
		style.PopupRounding = 3.0f;
		style.TabRounding = 3.0f;
		style.WindowRounding = 3.0f;

		ImGuiIO& io = ImGui::GetIO();
		//io.DisplaySize = ImVec2(static_cast<float>(pRenderTarget->GetExtent().mWidth), static_cast<float>(pRenderTarget->GetExtent().mHeight));
		//io.DisplayFramebufferScale = ImVec2(16.0f, 9.0f);
		//
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 12.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 8.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 10.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 14.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 16.0f);
		//
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/azonix-font/Azonix-1VB0.otf").string().c_str(), 10.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/a-atomic-md-font/AtomicMd-OVJ4A.otf").string().c_str(), 10.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/josefin-sans-font/JosefinSansRegular-x3LYV.ttf").string().c_str(), 12.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/salma-alfasans-font/SalmaalfasansLight-d9MJx.otf").string().c_str(), 12.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/swansea-font/Swansea-q3pd.ttf").string().c_str(), 6.0f);
		//io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/rawengulk-font/RawengulkBold-r8o9.otf").string().c_str(), 13.0f);
		//
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	}
}