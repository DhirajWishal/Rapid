// Copyright (c) 2022 Dhiraj Wishal

#define VMA_IMPLEMENTATION
#include "GraphicsEngine.hpp"
#include "Utility.hpp"

#include <SDL_vulkan.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <fstream>

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

#ifdef RAPID_DEBUG
		const auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
		vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

#endif

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

#endif

		// Create the instance.
		utility::ValidateResult(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance), "Failed to create the Vulkan instance!");

#ifdef RAPID_DEBUG
		const auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
		utility::ValidateResult(vkCreateDebugUtilsMessengerEXT(m_Instance, &debugMessengerCreateInfo, nullptr, &m_DebugMessenger), "Failed to create the debug messenger.");

#endif
	}

	void GraphicsEngine::selectPhysicalDevice()
	{
	}

	void GraphicsEngine::createLogicalDevice()
	{
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