// Copyright (c) 2022 Dhiraj Wishal

#include "ImGuiNode.hpp"
#include "Window.hpp"

#include <imgui.h>
#include <SDL.h>

#include <array>

using vec2 = std::array<float, 2>;

namespace
{
	constexpr uint64_t ElementCount = 2500;

	/**
	 * Get the new vertex buffer size.
	 * This will compute a bit more than what we actually need because then we don't have to recreate and update the vertex buffers all the time.
	 *
	 * @param newSize The new required size.
	 * @return The allocatable size.
	 */
	uint64_t GetNewVertexBufferSize(const uint64_t newSize)
	{
		constexpr uint64_t VertexFactor = ElementCount * sizeof(ImDrawVert);
		const auto count = (newSize / VertexFactor) + 1;
		return count * VertexFactor;
	}

	/**
	 * Get the new index buffer size.
	 * This will compute a bit more than what we actually need because then we don't have to recreate and update the index buffers all the time.
	 *
	 * @param newSize The new required size.
	 * @return The allocatable size.
	 */
	uint64_t GetNewIndexBufferSize(const uint64_t newSize)
	{
		constexpr uint64_t IndexFactor = ElementCount * sizeof(ImDrawIdx);
		const auto count = (newSize / IndexFactor) + 1;
		return count * IndexFactor;
	}

	/**
	 * Take one value and convert to vec2.
	 *
	 * @param x The x value.
	 * @return The vector.
	 */
	vec2 ToVec2(float x) { return { x, x }; }

	/**
	 * Take two float values and convert them to vec2.
	 *
	 * @param x The x value.
	 * @param y The y value.
	 * @return The vector.
	 */
	vec2 ToVec2(float x, float y) { return { x, y }; }
}

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

		m_FontImage = std::make_unique<Image>(m_Engine, VkExtent3D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1u }, VkFormat::VK_FORMAT_R8G8B8A8_UNORM, pFontImageData);
		m_FontImage->changeImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// Also set the window size.
		const auto windowExtent = m_Window.extent();
		imGuiIO.DisplaySize.x = windowExtent.width;
		imGuiIO.DisplaySize.y = windowExtent.height;

		// The vertex shader needs to be treated differently, because we need to switch data types.
		auto vertexShader = rapid::ShaderCode("Shaders/vert.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);
		vertexShader.m_InputAttributes[2].m_Size = 4;

		// Create the graphics pipeline.
		m_Pipeline = std::make_unique<GraphicsPipeline>(m_Engine, m_Window, "ImGuiPipelineCache.bin",
			vertexShader,
			rapid::ShaderCode("Shaders/frag.spv", VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT));

		// Setup shader resources.
		for (uint8_t i = 0; i < m_Window.frameCount(); i++)
		{
			auto& resource = m_ShaderResources.emplace_back(&m_Pipeline->createShaderResource());
			resource->bindResource(0, *m_FontImage);
		}

		// Create the vertex and index buffers.
		m_VertexBuffer = std::make_unique<Buffer>(m_Engine, GetNewVertexBufferSize(0), BufferType::ShallowVertex);
		m_IndexBuffer = std::make_unique<Buffer>(m_Engine, GetNewIndexBufferSize(0), BufferType::ShallowIndex);
	}

	ImGuiNode::~ImGuiNode()
	{
		if (isActive())
			terminate();
	}

	void ImGuiNode::terminate()
	{
		m_Pipeline->terminate();
		m_FontImage->terminate();
		m_IsTerminated = true;
	}

	void ImGuiNode::onPollEvents(SDL_Event& events)
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
		ImGui::DockSpace(ImGui::GetID("EditorDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

		// Transmit events to ImGui.
		{
			// Setup mouse data.
			int32_t mouseX, mouseY;
			const auto buttonState = SDL_GetMouseState(&mouseX, &mouseY);

			auto& imGuiIO = ImGui::GetIO();
			imGuiIO.MousePos.x = static_cast<float>(mouseX);
			imGuiIO.MousePos.y = static_cast<float>(mouseY);

			// Left mouse button press.
			if (buttonState & SDL_BUTTON_LEFT)
				imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
			else
				imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Left, false);

			// Right mouse button press.
			if (buttonState & SDL_BUTTON_RIGHT)
				imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Right, true);
			else
				imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Right, false);

			// Middle mouse button press.
			if (buttonState & SDL_BUTTON_MIDDLE)
				imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Middle, true);
			else
				imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Middle, false);

			// Set the scroll event.
			imGuiIO.AddMouseWheelEvent(events.wheel.preciseX, events.wheel.preciseY);
		}
	}

	void ImGuiNode::bind(CommandBuffer commandBuffer, uint32_t frameIndex)
	{
		ImGui::End();
		ImGui::Render();

		// Update the buffers.
		updateBuffers();

		ImGuiIO& imGuiIO = ImGui::GetIO();
		ImDrawData* pDrawData = ImGui::GetDrawData();

		if (!pDrawData)
			return;

		// Update and Render additional Platform Windows
		if (imGuiIO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
#ifdef RAPID_PLATFORM_WINDOWS
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

#endif
		}

		// Setup push constants.
		struct PushConstants final
		{
			vec2 m_Scale = ToVec2(1.0f);
			vec2 m_Translate = ToVec2(1.0f);
		} pushConstants;

		pushConstants.m_Scale = ToVec2(2.0f / imGuiIO.DisplaySize.x, 2.0f / imGuiIO.DisplaySize.y);
		pushConstants.m_Translate = ToVec2(-1.0f);

		// Set the viewport.
		const VkViewport viewport = {
			.x = 0.0f,
			.y = 0.0f,
			.width = imGuiIO.DisplaySize.x,
			.height = imGuiIO.DisplaySize.y,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		// Issue draw calls.
		if (pDrawData->CmdListsCount)
		{
			commandBuffer.bindVertexBuffer(*m_VertexBuffer);
			commandBuffer.bindIndexBuffer(*m_IndexBuffer, VkIndexType::VK_INDEX_TYPE_UINT16);
			commandBuffer.bindPipeline(*m_Pipeline);

			uint64_t vertexOffset = 0, indexOffset = 0;
			for (int32_t i = 0; i < pDrawData->CmdListsCount; i++)
			{
				const auto pCommandList = pDrawData->CmdLists[i];

				for (int32_t j = 0; j < pCommandList->CmdBuffer.Size; j++)
				{
					const auto& pCommand = pCommandList->CmdBuffer[j];

					// Setup scissor.
					const VkRect2D scissor = {
						.offset = {
							.x = std::max(static_cast<int32_t>(pCommand.ClipRect.x), 0),
							.y = std::max(static_cast<int32_t>(pCommand.ClipRect.y), 0),
						},
						.extent = {
							.width = static_cast<uint32_t>(pCommand.ClipRect.z - pCommand.ClipRect.x),
							.height = static_cast<uint32_t>(pCommand.ClipRect.w - pCommand.ClipRect.y),
						}
					};

					// Bind all the resources.
					commandBuffer.bindShaderResource(*m_Pipeline, *m_ShaderResources[frameIndex]);
					commandBuffer.bindViewport(viewport);
					commandBuffer.bindScissor(scissor);
					commandBuffer.bindPushConstant(*m_Pipeline, &pushConstants, sizeof(PushConstants), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT);

					// Issue the draw call.
					commandBuffer.drawIndices(pCommand.ElemCount, indexOffset, vertexOffset);

					indexOffset += pCommand.ElemCount;
				}

				vertexOffset += pCommandList->VtxBuffer.Size;
			}
		}
	}

	void ImGuiNode::onWindowResize()
	{
		m_Pipeline->recreate();

		// Set the new window size.
		const auto extent = m_Window.extent();
		ImGuiIO& imGuiIO = ImGui::GetIO();
		imGuiIO.DisplaySize.x = static_cast<float>(extent.width);
		imGuiIO.DisplaySize.y = static_cast<float>(extent.height);
	}

	void ImGuiNode::updateBuffers()
	{
		ImDrawData* pDrawData = ImGui::GetDrawData();

		// We don't have to update anything if there are no 
		if (!pDrawData)
			return;

		// Get the vertex and index size and return if we don't have anything.
		const uint64_t vertexSize = GetNewVertexBufferSize(pDrawData->TotalVtxCount * sizeof(ImDrawVert)), indexSize = GetNewIndexBufferSize(pDrawData->TotalIdxCount * sizeof(ImDrawIdx));
		if (vertexSize == 0 || indexSize == 0)
			return;

		const auto currentVertexCount = m_VertexBuffer->size() / sizeof(ImDrawVert);
		const auto currentIndexCount = m_IndexBuffer->size() / sizeof(ImDrawIdx);

		// Create buffers if we need to.
		if (currentVertexCount < pDrawData->TotalVtxCount || pDrawData->TotalVtxCount < (currentVertexCount - ElementCount))
		{
			m_VertexBuffer->terminate();
			m_VertexBuffer = std::make_unique<Buffer>(m_Engine, GetNewVertexBufferSize(vertexSize), BufferType::ShallowVertex);
		}

		if (currentIndexCount < pDrawData->TotalIdxCount || pDrawData->TotalIdxCount < (currentIndexCount - ElementCount))
		{
			m_IndexBuffer->terminate();
			m_IndexBuffer = std::make_unique<Buffer>(m_Engine, GetNewIndexBufferSize(indexSize), BufferType::ShallowIndex);
		}

		// Copy the content.
		auto pCopyVertexPointer = reinterpret_cast<ImDrawVert*>(m_VertexBuffer->mapMemory());
		auto pCopyIndexPointer = reinterpret_cast<ImDrawIdx*>(m_IndexBuffer->mapMemory());
		for (int32_t i = 0; i < pDrawData->CmdListsCount; i++) {
			const auto pCommandList = pDrawData->CmdLists[i];

			std::copy_n(pCommandList->VtxBuffer.Data, pCommandList->VtxBuffer.Size, pCopyVertexPointer);
			std::copy_n(pCommandList->IdxBuffer.Data, pCommandList->IdxBuffer.Size, pCopyIndexPointer);

			pCopyVertexPointer += pCommandList->VtxBuffer.Size;
			pCopyIndexPointer += pCommandList->IdxBuffer.Size;
		}

		// Unmap the mapped memory.
		m_VertexBuffer->unmapMemory();
		m_IndexBuffer->unmapMemory();
	}
}