// Copyright (c) 2022 Dhiraj Wishal

#include "NodeEditor.hpp"

#include <imgui.h>
#include <imnodes.h>

namespace
{
	/**
	 * Set the mini map hovered callback.
	 *
	 * @param nodeID The ID of the node.
	 * @param pUserData The user data associated with the node.
	 */
	void MiniMapHoveredCallback(int32_t nodeID, void* pUserData)
	{
		ImGui::SetTooltip("This is node %d", nodeID);
	}
}

namespace rapid
{
	NodeBuilder::NodeBuilder(std::string_view title, const int32_t nodeID)
		: m_Title(title), m_NodeID(nodeID)
	{
	}

	void NodeBuilder::addInputAttribute(std::string_view name, const int32_t ID)
	{
		m_InputAttributes.emplace_back(name, ID);
	}

	void NodeBuilder::addOutputAttribute(std::string_view name, const int32_t ID)
	{
		m_OutputAttributes.emplace_back(name, ID);
	}

	void NodeBuilder::show() const
	{
		ImNodes::BeginNode(m_NodeID);

		// Show the title.
		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted(m_Title.data());
		ImNodes::EndNodeTitleBar();

		// Get the IO information.
		auto& imNodesIO = ImNodes::GetIO();

		// Let's render all the input attributes.
		for (const auto& attribute : m_InputAttributes)
		{
			// Make sure to push the attribute flag so we can detach the link.
			ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

			// Set the attribute info.
			ImNodes::BeginInputAttribute(attribute.m_AttributeID, ImNodesPinShape_TriangleFilled);
			ImGui::Text(attribute.m_AttributeName.data());
			ImNodes::EndInputAttribute();

			// Don't forget to pop the attribute flag!
			ImNodes::PopAttributeFlag();
		}

		// Now let's render all the output attributes.
		for (const auto& attribute : m_OutputAttributes)
		{
			// Make sure to push the attribute flag so we can detach the link.
			ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

			// Set the attribute info.
			ImNodes::BeginOutputAttribute(attribute.m_AttributeID, ImNodesPinShape_TriangleFilled);
			ImGui::Text(attribute.m_AttributeName.data());
			ImNodes::EndOutputAttribute();

			// Don't forget to pop the attribute flag!
			ImNodes::PopAttributeFlag();
		}

		// Let's end the node.
		ImNodes::EndNode();
	}

	NodeEditor::NodeEditor()
		: UIComponent("Node Editor")
	{
		ImNodes::CreateContext();

		// Set global styles.
		auto& style = ImNodes::GetStyle();
		style.Colors[ImNodesCol_TitleBar] = IM_COL32(227, 66, 52, 128);
		style.Colors[ImNodesCol_TitleBarHovered] = IM_COL32(227, 99, 85, 255);
		style.Colors[ImNodesCol_TitleBarSelected] = IM_COL32(227, 66, 52, 255);
	}

	NodeEditor::~NodeEditor()
	{
		ImNodes::DestroyContext();
	}

	rapid::NodeBuilder& NodeEditor::createNode(std::string_view title)
	{
		return m_NodeBuilders.emplace_back(title, m_NodeID++);
	}

	void NodeEditor::begin()
	{
		// First show all the nodes.
		ImGui::Begin("Nodes");

		// Create new node if possible.
		static bool selected = false;
		if (ImGui::Button("Create New") || selected)
			selected = CreateNewNode();

		for (const auto& node : m_NodeBuilders)
		{
			if (ImGui::Selectable(node.getTitle().data()))
			{
				// TODO
			}
		}
		ImGui::End();

		// Now we can begin the editor.
		ImGui::Begin(m_Title.c_str());
		ImNodes::BeginNodeEditor();

		// Finally we can show the nodes.
		for (const auto& node : m_NodeBuilders)
			node.show();
	}

	void NodeEditor::end()
	{
		// Create the links if we have any.
		for (int i = 0; i < m_Links.size(); ++i)
		{
			const std::pair<int, int> p = m_Links[i];
			ImNodes::Link(i, p.first, p.second);
		}

		// Make sure to show the mini map before we end!
		ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight, MiniMapHoveredCallback);

		//ImNodes::PopAttributeFlag();
		ImNodes::EndNodeEditor();
		ImGui::End();

		// Resolve the links.
		int32_t startLink, endLink;
		if (ImNodes::IsLinkCreated(&startLink, &endLink))
			m_Links.emplace_back(startLink, endLink);

		// Remove links if we have to.
		int32_t link;
		if (ImNodes::IsLinkDestroyed(&link))
			m_Links.erase(m_Links.begin() + link);

		// Show what the link is when hovered. TODO
		int32_t hoveredLink;
		if (ImNodes::IsLinkHovered(&hoveredLink))
			ImGui::SetTooltip("Link");
	}

	bool NodeEditor::CreateNewNode()
	{
		ImGui::Begin("Create New Node");

		if (ImGui::InputText("Node name: ", m_NewNodeNameBuffer, 256))
		{
			ImGui::Text("Input!");
		}

		bool isCreated = ImGui::Button("Create");
		ImGui::End();
		return !isCreated;
	}
}