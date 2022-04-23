// Copyright (c) 2022 Dhiraj Wishal

#include "NodeEditor.hpp"
#include "Console.hpp"

#include <imgui.h>
#include <imnodes.h>

#ifdef RAPID_PLATFORM_WINDOWS
#	include <execution>

#endif

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
		const auto pNodes = reinterpret_cast<rapid::NodeBuilder*>(pUserData);
		ImGui::SetTooltip("This is node %s", pNodes[nodeID].getTitle().data());
	}
}

namespace rapid
{
	NodeBuilder::NodeBuilder(std::string title, const int32_t nodeID, int32_t& attributeID)
		: m_Title(std::move(title)), m_AttributeID(attributeID), m_NodeID(nodeID)
	{
	}

	void NodeBuilder::addInputAttribute(std::string name)
	{
		m_InputAttributes.emplace_back(std::move(name), m_AttributeID++);
	}

	void NodeBuilder::addOutputAttribute(std::string name)
	{
		m_OutputAttributes.emplace_back(std::move(name), m_AttributeID++);
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

	rapid::NodeBuilder& NodeEditor::createNode(std::string title)
	{
		return m_NodeBuilders.emplace_back(title, m_NodeID++, m_NodeAttributeID);
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
		ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight, MiniMapHoveredCallback, m_NodeBuilders.data());

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
		ImGui::InputText("Node name", m_NewNodeNameBuffer, IM_ARRAYSIZE(m_NewNodeNameBuffer));

		// Get the input names.
		ImGui::InputInt("Input count", &m_NewNodeInputCount);
		m_NewNodeInputNames.resize(m_NewNodeInputCount);
		for (int32_t i = 0; i < m_NewNodeInputCount; i++)
			ImGui::InputText(("Input " + std::to_string(i)).c_str(), m_NewNodeInputNames[i].data(), m_NewNodeInputNames[i].size());

		// Get the output names.
		ImGui::InputInt("Output count", &m_NewNodeOutputCount);
		m_NewNodeOutputNames.resize(m_NewNodeOutputCount);
		for (int32_t i = 0; i < m_NewNodeOutputCount; i++)
			ImGui::InputText(("Output " + std::to_string(i)).c_str(), m_NewNodeOutputNames[i].data(), m_NewNodeOutputNames[i].size());

		// We can try to create if the user wants to.
		if (ImGui::Button("Create"))
		{
			// Stop it here because we anyway have to.
			ImGui::End();

			// Check if we have data, if not we can return.
			if (!m_NewNodeNameBuffer[0])
			{
				GetConsole().log("Failed to create a new node! Make sure that you have data to create the node first.", Severity::Warning);
				return true;
			}

			auto& node = createNode(m_NewNodeNameBuffer);

			// Create the input attributes.
			for (auto& name : m_NewNodeInputNames)
				node.addInputAttribute(name.data());

			// Create the output attributes.
			for (auto& name : m_NewNodeOutputNames)
				node.addOutputAttribute(name.data());

			// Make sure to clear the values before we leave!
			cleanupNewNodeData();

			return false;
		}

		// Cancel if the user wants to.
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			ImGui::End();
			cleanupNewNodeData();
			return false;
		}

		// Or we can clear.
		ImGui::SameLine();
		if (ImGui::Button("Clear")) cleanupNewNodeData();

		ImGui::End();
		return true;
	}

	void NodeEditor::cleanupNewNodeData()
	{
		m_NewNodeInputNames.clear();
		m_NewNodeOutputNames.clear();

#ifdef RAPID_PLATFORM_WINDOWS
		std::fill_n(std::execution::unseq, m_NewNodeNameBuffer, IM_ARRAYSIZE(m_NewNodeNameBuffer), 0);

#else
		std::fill_n(m_NewNodeNameBuffer, IM_ARRAYSIZE(m_NewNodeNameBuffer), 0);

#endif

		m_NewNodeInputCount = 0;
		m_NewNodeOutputCount = 0;
	}
}