// Copyright (c) 2022 Dhiraj Wishal

#include "ImGuiNode.hpp"
#include "Window.hpp"

#include <imgui.h>

namespace rapid
{
	ImGuiNode::ImGuiNode(GraphicsEngine& engine, Window& window)
		: ProcessingNode(engine, window)
	{
		// Load the font image to a Vulkan image.
		std::byte* pFontImageData = nullptr;
		int32_t width = 0, height = 0, bitsPerPixel = 0;

		ImGuiIO& imGuiIO = ImGui::GetIO();
		imGuiIO.Fonts->GetTexDataAsRGBA32(reinterpret_cast<uint8_t**>(&pFontImageData), &width, &height, &bitsPerPixel);

		m_FontImage = std::make_unique<Image>(m_Engine, VkExtent3D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1u }, VkFormat::VK_FORMAT_R8G8B8A8_SRGB, pFontImageData);

		// Also set the window size.
		const auto windowExtent = m_Window.extent();
		imGuiIO.DisplaySize.x = windowExtent.width;
		imGuiIO.DisplaySize.y = windowExtent.height;
	}

	ImGuiNode::~ImGuiNode()
	{
		if (isActive())
			terminate();
	}

	void ImGuiNode::terminate()
	{
		m_FontImage->terminate();
		m_IsTerminated = true;
	}

	void ImGuiNode::onPollEvents()
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

	void ImGuiNode::bind(CommandBuffer commandBuffer)
	{
		ImGui::End();
		ImGui::Render();
	}
}