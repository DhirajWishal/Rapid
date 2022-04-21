// Copyright (c) 2022 Dhiraj Wishal

#include "ImGuiNode.hpp"

#include <imgui.h>

namespace rapid
{
	ImGuiNode::ImGuiNode(GraphicsEngine& engine)
		: ProcessingNode(engine)
	{
		// Load the font image to a Vulkan image.
		std::byte* pFontImageData = nullptr;
		int32_t width = 0, height = 0, bitsPerPixel = 0;

		ImGuiIO& imGuiIO = ImGui::GetIO();
		imGuiIO.Fonts->GetTexDataAsRGBA32(reinterpret_cast<uint8_t**>(&pFontImageData), &width, &height, &bitsPerPixel);

		m_FontImage = std::make_unique<Image>(m_Engine, VkExtent3D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1u }, VkFormat::VK_FORMAT_R8G8B8A8_SRGB, pFontImageData);
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

	void ImGuiNode::bind(VkCommandBuffer vCommandBuffer)
	{

	}
}