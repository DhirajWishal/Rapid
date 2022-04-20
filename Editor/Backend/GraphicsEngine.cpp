// Copyright (c) 2022 Dhiraj Wishal

#define VMA_IMPLEMENTATION
#include "GraphicsEngine.hpp"
#include "Utility.hpp"

#include <imgui.h>

namespace
{
	/**
	 * Create a color from the float value from 0 - 255.
	 *
	 * @param value The color value.
	 * @return The created color value.
	 */
	consteval float CreateColor256(float value) { return value / 256; }
}

namespace rapid
{
	GraphicsEngine::GraphicsEngine()
	{
		// FIrst of all, initialize volk. Without this we can't do anything else.
		utility::ValidateResult(volkInitialize(), "Failed to initialize volk!");

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
		VkApplicationInfo applicationInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "Rapid",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = volkGetInstanceVersion()
		};
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