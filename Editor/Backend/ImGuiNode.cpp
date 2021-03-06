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

		// Resolve the time delta.
		auto newTime = clock_type::now();
		const auto diff = newTime - m_TimePoint;

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
			auto& imGuiIO = ImGui::GetIO();
			imGuiIO.Framerate = std::nano::den / diff.count();
			imGuiIO.DeltaTime = diff.count() / static_cast<float>(std::nano::den);

			switch (events.type)
			{
			case SDL_KEYDOWN:
				resolveKeyboardInputs(events.key.keysym.scancode, true);
				//imGuiIO.AddKeyEvent(ImGuiKey_LeftCtrl, events.key.keysym.mod & KMOD_LCTRL);
				//imGuiIO.AddKeyEvent(ImGuiKey_LeftShift, events.key.keysym.mod & KMOD_LSHIFT);
				//imGuiIO.AddKeyEvent(ImGuiKey_LeftAlt, events.key.keysym.mod & KMOD_LALT);
				//
				//imGuiIO.AddKeyEvent(ImGuiKey_RightCtrl, events.key.keysym.mod & KMOD_RCTRL);
				//imGuiIO.AddKeyEvent(ImGuiKey_RightShift, events.key.keysym.mod & KMOD_RSHIFT);
				//imGuiIO.AddKeyEvent(ImGuiKey_RightAlt, events.key.keysym.mod & KMOD_RALT);

				if (events.key.keysym.mod & KMOD_CTRL)
					imGuiIO.KeyMods |= ImGuiModFlags_Ctrl;

				if (events.key.keysym.mod & KMOD_SHIFT)
					imGuiIO.KeyMods |= ImGuiModFlags_Shift;

				if (events.key.keysym.mod & KMOD_ALT)
					imGuiIO.KeyMods |= ImGuiModFlags_Alt;

				if (events.key.keysym.mod & KMOD_GUI)
					imGuiIO.KeyMods |= ImGuiModFlags_Super;

				break;

			case SDL_KEYUP:
				resolveKeyboardInputs(events.key.keysym.scancode, false);
				//imGuiIO.AddKeyEvent(ImGuiKey_LeftCtrl, events.key.keysym.mod & KMOD_LCTRL);
				//imGuiIO.AddKeyEvent(ImGuiKey_LeftShift, events.key.keysym.mod & KMOD_LSHIFT);
				//imGuiIO.AddKeyEvent(ImGuiKey_LeftAlt, events.key.keysym.mod & KMOD_LALT);
				//
				//imGuiIO.AddKeyEvent(ImGuiKey_RightCtrl, events.key.keysym.mod & KMOD_RCTRL);
				//imGuiIO.AddKeyEvent(ImGuiKey_RightShift, events.key.keysym.mod & KMOD_RSHIFT);
				//imGuiIO.AddKeyEvent(ImGuiKey_RightAlt, events.key.keysym.mod & KMOD_RALT);

				//if (events.key.keysym.mod & KMOD_CTRL)
				//	imGuiIO.KeyMods |= ImGuiModFlags_Ctrl;
				//
				//if (events.key.keysym.mod & KMOD_SHIFT)
				//	imGuiIO.KeyMods |= ImGuiModFlags_Shift;
				//
				//if (events.key.keysym.mod & KMOD_ALT)
				//	imGuiIO.KeyMods |= ImGuiModFlags_Alt;
				//
				//if (events.key.keysym.mod & KMOD_GUI)
				//	imGuiIO.KeyMods |= ImGuiModFlags_Super;

				break;

			case SDL_TEXTINPUT:
				imGuiIO.AddInputCharactersUTF8(events.text.text);
				break;

			case SDL_MOUSEBUTTONDOWN:
				// Left mouse button press.
				if (events.button.button == SDL_BUTTON_LEFT)
				{
					imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
					imGuiIO.MouseClickedCount[ImGuiMouseButton_Left] = events.button.clicks;
				}


				// Right mouse button press.
				if (events.button.button == SDL_BUTTON_RIGHT)
				{
					imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Right, true);
					imGuiIO.MouseClickedCount[ImGuiMouseButton_Right] = events.button.clicks;
				}

				// Middle mouse button press.
				if (events.button.button == SDL_BUTTON_MIDDLE)
				{
					imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Middle, true);
					imGuiIO.MouseClickedCount[ImGuiMouseButton_Middle] = events.button.clicks;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				// Left mouse button release.
				if (events.button.button == SDL_BUTTON_LEFT)
					imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Left, false);

				// Right mouse button release.
				if (events.button.button == SDL_BUTTON_RIGHT)
					imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Right, false);

				// Middle mouse button release.
				if (events.button.button == SDL_BUTTON_MIDDLE)
					imGuiIO.AddMouseButtonEvent(ImGuiMouseButton_Middle, false);
				break;

			case SDL_MOUSEMOTION:
				imGuiIO.MousePos.x = static_cast<float>(events.motion.x);
				imGuiIO.MousePos.y = static_cast<float>(events.motion.y);
				break;

			case SDL_MOUSEWHEEL:
				imGuiIO.AddMouseWheelEvent(events.wheel.preciseX, events.wheel.preciseY);
				break;

			default:
				break;
			}
		}

		m_TimePoint = newTime;
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

	void ImGuiNode::resolveKeyboardInputs(SDL_Scancode scancode, bool state) const
	{
		ImGuiKey imGuiKey = 0;
		switch (scancode)
		{
		case SDL_SCANCODE_RETURN:			imGuiKey = ImGuiKey_Enter; break;
		case SDL_SCANCODE_ESCAPE:			imGuiKey = ImGuiKey_Escape; break;
		case SDL_SCANCODE_BACKSPACE:		imGuiKey = ImGuiKey_Backspace; break;
		case SDL_SCANCODE_TAB:				imGuiKey = ImGuiKey_Tab; break;
		case SDL_SCANCODE_SPACE:			imGuiKey = ImGuiKey_Space; break;

		case SDL_SCANCODE_A:				imGuiKey = ImGuiKey_A; break;
		case SDL_SCANCODE_B:				imGuiKey = ImGuiKey_B; break;
		case SDL_SCANCODE_C:				imGuiKey = ImGuiKey_C; break;
		case SDL_SCANCODE_D:				imGuiKey = ImGuiKey_D; break;
		case SDL_SCANCODE_E:				imGuiKey = ImGuiKey_E; break;
		case SDL_SCANCODE_F:				imGuiKey = ImGuiKey_F; break;
		case SDL_SCANCODE_G:				imGuiKey = ImGuiKey_G; break;
		case SDL_SCANCODE_H:				imGuiKey = ImGuiKey_H; break;
		case SDL_SCANCODE_I:				imGuiKey = ImGuiKey_I; break;
		case SDL_SCANCODE_J:				imGuiKey = ImGuiKey_J; break;
		case SDL_SCANCODE_K:				imGuiKey = ImGuiKey_K; break;
		case SDL_SCANCODE_L:				imGuiKey = ImGuiKey_L; break;
		case SDL_SCANCODE_M:				imGuiKey = ImGuiKey_M; break;
		case SDL_SCANCODE_N:				imGuiKey = ImGuiKey_N; break;
		case SDL_SCANCODE_O:				imGuiKey = ImGuiKey_O; break;
		case SDL_SCANCODE_P:				imGuiKey = ImGuiKey_P; break;
		case SDL_SCANCODE_Q:				imGuiKey = ImGuiKey_Q; break;
		case SDL_SCANCODE_R:				imGuiKey = ImGuiKey_R; break;
		case SDL_SCANCODE_S:				imGuiKey = ImGuiKey_S; break;
		case SDL_SCANCODE_T:				imGuiKey = ImGuiKey_T; break;
		case SDL_SCANCODE_U:				imGuiKey = ImGuiKey_U; break;
		case SDL_SCANCODE_V:				imGuiKey = ImGuiKey_V; break;
		case SDL_SCANCODE_W:				imGuiKey = ImGuiKey_W; break;
		case SDL_SCANCODE_X:				imGuiKey = ImGuiKey_X; break;
		case SDL_SCANCODE_Y:				imGuiKey = ImGuiKey_Y; break;
		case SDL_SCANCODE_Z:				imGuiKey = ImGuiKey_Z; break;

		case SDL_SCANCODE_1:				imGuiKey = ImGuiKey_1; break;
		case SDL_SCANCODE_2:				imGuiKey = ImGuiKey_2; break;
		case SDL_SCANCODE_3:				imGuiKey = ImGuiKey_3; break;
		case SDL_SCANCODE_4:				imGuiKey = ImGuiKey_4; break;
		case SDL_SCANCODE_5:				imGuiKey = ImGuiKey_5; break;
		case SDL_SCANCODE_6:				imGuiKey = ImGuiKey_6; break;
		case SDL_SCANCODE_7:				imGuiKey = ImGuiKey_7; break;
		case SDL_SCANCODE_8:				imGuiKey = ImGuiKey_8; break;
		case SDL_SCANCODE_9:				imGuiKey = ImGuiKey_9; break;
		case SDL_SCANCODE_0:				imGuiKey = ImGuiKey_0; break;

		case SDL_SCANCODE_MINUS:			imGuiKey = ImGuiKey_Minus; break;
		case SDL_SCANCODE_EQUALS:			imGuiKey = ImGuiKey_Equal; break;
		case SDL_SCANCODE_LEFTBRACKET:		imGuiKey = ImGuiKey_LeftBracket; break;
		case SDL_SCANCODE_RIGHTBRACKET:		imGuiKey = ImGuiKey_RightBracket; break;
		case SDL_SCANCODE_BACKSLASH:		imGuiKey = ImGuiKey_Backslash; break;

		case SDL_SCANCODE_SEMICOLON:		imGuiKey = ImGuiKey_Semicolon; break;
		case SDL_SCANCODE_APOSTROPHE:		imGuiKey = ImGuiKey_Apostrophe; break;
		case SDL_SCANCODE_GRAVE:			imGuiKey = ImGuiKey_GraveAccent; break;

		case SDL_SCANCODE_COMMA:			imGuiKey = ImGuiKey_Comma; break;
		case SDL_SCANCODE_PERIOD:			imGuiKey = ImGuiKey_Period; break;
		case SDL_SCANCODE_SLASH:			imGuiKey = ImGuiKey_Slash; break;

		case SDL_SCANCODE_CAPSLOCK:			imGuiKey = ImGuiKey_CapsLock; break;

		case SDL_SCANCODE_F1:				imGuiKey = ImGuiKey_F1; break;
		case SDL_SCANCODE_F2:				imGuiKey = ImGuiKey_F2; break;
		case SDL_SCANCODE_F3:				imGuiKey = ImGuiKey_F3; break;
		case SDL_SCANCODE_F4:				imGuiKey = ImGuiKey_F4; break;
		case SDL_SCANCODE_F5:				imGuiKey = ImGuiKey_F5; break;
		case SDL_SCANCODE_F6:				imGuiKey = ImGuiKey_F6; break;
		case SDL_SCANCODE_F7:				imGuiKey = ImGuiKey_F7; break;
		case SDL_SCANCODE_F8:				imGuiKey = ImGuiKey_F8; break;
		case SDL_SCANCODE_F9:				imGuiKey = ImGuiKey_F9; break;
		case SDL_SCANCODE_F10:				imGuiKey = ImGuiKey_F10; break;
		case SDL_SCANCODE_F11:				imGuiKey = ImGuiKey_F11; break;
		case SDL_SCANCODE_F12:				imGuiKey = ImGuiKey_F12; break;

		case SDL_SCANCODE_PRINTSCREEN:		imGuiKey = ImGuiKey_PrintScreen; break;
		case SDL_SCANCODE_SCROLLLOCK:		imGuiKey = ImGuiKey_ScrollLock; break;
		case SDL_SCANCODE_PAUSE:			imGuiKey = ImGuiKey_Pause; break;
		case SDL_SCANCODE_INSERT:			imGuiKey = ImGuiKey_Insert; break;

		case SDL_SCANCODE_HOME:				imGuiKey = ImGuiKey_Home; break;
		case SDL_SCANCODE_PAGEUP:			imGuiKey = ImGuiKey_PageUp; break;
		case SDL_SCANCODE_DELETE:			imGuiKey = ImGuiKey_PageDown; break;
		case SDL_SCANCODE_END:				imGuiKey = ImGuiKey_End; break;
		case SDL_SCANCODE_PAGEDOWN:			imGuiKey = ImGuiKey_PageDown; break;
		case SDL_SCANCODE_RIGHT:			imGuiKey = ImGuiKey_RightArrow; break;
		case SDL_SCANCODE_LEFT:				imGuiKey = ImGuiKey_LeftArrow; break;
		case SDL_SCANCODE_DOWN:				imGuiKey = ImGuiKey_DownArrow; break;
		case SDL_SCANCODE_UP:				imGuiKey = ImGuiKey_UpArrow; break;

		case SDL_SCANCODE_NUMLOCKCLEAR:		imGuiKey = ImGuiKey_NumLock; break;

		case SDL_SCANCODE_KP_DIVIDE:		imGuiKey = ImGuiKey_KeypadDivide; break;
		case SDL_SCANCODE_KP_MULTIPLY:		imGuiKey = ImGuiKey_KeypadMultiply; break;
		case SDL_SCANCODE_KP_MINUS:			imGuiKey = ImGuiKey_KeypadSubtract; break;
		case SDL_SCANCODE_KP_PLUS:			imGuiKey = ImGuiKey_KeypadAdd; break;
		case SDL_SCANCODE_KP_ENTER:			imGuiKey = ImGuiKey_KeypadEnter; break;
		case SDL_SCANCODE_KP_1:				imGuiKey = ImGuiKey_Keypad1; break;
		case SDL_SCANCODE_KP_2:				imGuiKey = ImGuiKey_Keypad2; break;
		case SDL_SCANCODE_KP_3:				imGuiKey = ImGuiKey_Keypad3; break;
		case SDL_SCANCODE_KP_4:				imGuiKey = ImGuiKey_Keypad4; break;
		case SDL_SCANCODE_KP_5:				imGuiKey = ImGuiKey_Keypad5; break;
		case SDL_SCANCODE_KP_6:				imGuiKey = ImGuiKey_Keypad6; break;
		case SDL_SCANCODE_KP_7:				imGuiKey = ImGuiKey_Keypad7; break;
		case SDL_SCANCODE_KP_8:				imGuiKey = ImGuiKey_Keypad8; break;
		case SDL_SCANCODE_KP_9:				imGuiKey = ImGuiKey_Keypad9; break;
		case SDL_SCANCODE_KP_0:				imGuiKey = ImGuiKey_Keypad0; break;
		case SDL_SCANCODE_KP_PERIOD:		imGuiKey = ImGuiKey_KeypadDecimal; break;
		default:							break;
		}

		ImGui::GetIO().AddKeyEvent(imGuiKey, state);
	}
}