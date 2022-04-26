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
		ImGui::SetTooltip(pNodes[nodeID].getTitle().data());
	}
}

namespace rapid
{
	NodeBuilder::NodeBuilder(std::string title, const int32_t nodeID, int32_t& attributeID, NodeType type, uint32_t titleColor, uint32_t titleHoveredColor, uint32_t titleSelectedColor)
		: m_Title(std::move(title)), m_AttributeID(attributeID)
		, m_NodeID(nodeID), m_Type(type), m_TitleColor(titleColor)
		, m_TitleHoveredColor(titleHoveredColor), m_TitleSelectedColor(titleSelectedColor)
	{
	}

	void NodeBuilder::addInputAttribute(std::string name)
	{
		m_InputAttributes.emplace_back(std::move(name), m_AttributeID++, -1);
	}

	void NodeBuilder::addOutputAttribute(std::string name, int8_t prop)
	{
		m_OutputAttributes.emplace_back(std::move(name), m_AttributeID++, prop);
	}

	void NodeBuilder::show() const
	{
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, m_TitleColor);
		ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, m_TitleHoveredColor);
		ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, m_TitleSelectedColor);

		// Begin and show the title.
		ImNodes::BeginNode(m_NodeID);
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

			// Push the property colors.
			if (attribute.m_Property == -1)
			{
				ImNodes::PushColorStyle(ImNodesCol_Pin, ImNodes::GetStyle().Colors[ImNodesCol_Pin]);
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ImNodes::GetStyle().Colors[ImNodesCol_PinHovered]);
			}
			else if (attribute.m_Property == 0)
			{
				ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(0, 255, 0, 196));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, IM_COL32(0, 255, 0, 255));
			}
			else if (attribute.m_Property == 1)
			{
				ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(255, 0, 0, 196));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, IM_COL32(255, 0, 0, 255));
			}
			else if (attribute.m_Property == 2)
			{
				ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(0, 0, 255, 196));
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, IM_COL32(0, 0, 255, 255));
			}

			// Set the attribute info.
			ImNodes::BeginOutputAttribute(attribute.m_AttributeID, ImNodesPinShape_TriangleFilled);
			ImGui::Text(attribute.m_AttributeName.data());
			ImNodes::EndOutputAttribute();

			// Pop the color styles.
			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();

			// Don't forget to pop the attribute flag!
			ImNodes::PopAttributeFlag();
		}

		// Let's end the node.
		ImNodes::EndNode();

		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
		ImNodes::PopColorStyle();
	}

	NodeBuilder NodeBuilder::clone(const int32_t nodeID) const
	{
		auto newNode = NodeBuilder(m_Title, nodeID, m_AttributeID, m_Type, m_TitleColor, m_TitleHoveredColor, m_TitleSelectedColor);

		for (const auto& attribute : m_InputAttributes)
			newNode.addInputAttribute(attribute.m_AttributeName);

		for (const auto& attribute : m_OutputAttributes)
			newNode.addOutputAttribute(attribute.m_AttributeName);

		return newNode;
	}

	NodeEditor::NodeEditor()
		: UIComponent("Node Editor")
	{
		ImNodes::CreateContext();

		// Try and get the previous state.
		ImNodes::LoadCurrentEditorStateFromIniFile("editor.ini");
	}

	NodeEditor::~NodeEditor()
	{

		ImNodes::SaveCurrentEditorStateToIniFile("editor.ini");
		ImNodes::DestroyContext();
	}

	void NodeEditor::begin()
	{
		// Begin the editor.
		ImGui::Begin(m_Title.c_str());
		ImNodes::BeginNodeEditor();

		// If the user right-clicks, let's show the menu.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
		if (ImGui::BeginPopupContextWindow("Options", ImGuiPopupFlags_MouseButtonRight))
		{
			// Classes
			if (ImGui::Selectable("Create class")) m_ShouldCreateClass = true;
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Create a new class node. Classes gets a pre-defined 'this' member which will link to member methods/ functions.");
			}

			for (const auto& node : m_ClassNodeBuilders)
			{
				if (ImGui::Selectable(node.getTitle().data()))
					m_ActiveNodeBuilders.emplace_back(node.clone(m_NodeID++));
			}

			// Structs.
			ImGui::Separator();
			if (ImGui::Selectable("Create struct")) m_ShouldCreateStruct = true;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Create a new struct node. Classes gets a pre-defined 'this' member which will link to member methods/ functions.");
			for (const auto& node : m_StructNodeBuilders)
			{
				if (ImGui::Selectable(node.getTitle().data()))
					m_ActiveNodeBuilders.emplace_back(node.clone(m_NodeID++));
			}

			// Member functions.
			ImGui::Separator();
			if (ImGui::Selectable("Create member function/ node")) m_ShouldCreateMemberFunction = true;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Create a new member method/ function node. Member methods/ functions gets a pre-defined 'this' input which will link to the parent class/ struct it belongs to.");
			for (const auto& node : m_MemberNodeBuilders)
			{
				if (ImGui::Selectable(node.getTitle().data()))
					m_ActiveNodeBuilders.emplace_back(node.clone(m_NodeID++));
			}

			// Normal functions.
			ImGui::Separator();
			if (ImGui::Selectable("Create function/ node")) m_ShouldCreateFunction = true;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Create a new function. These are normal functions.");
			for (const auto& node : m_NodeBuilders)
			{
				if (ImGui::Selectable(node.getTitle().data()))
					m_ActiveNodeBuilders.emplace_back(node.clone(m_NodeID++));
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();

		// If we should, let's create a new node(s).
		if (m_ShouldCreateClass)
			m_ShouldCreateClass = createNewClass();

		if (m_ShouldCreateStruct)
			m_ShouldCreateStruct = createNewStruct();

		if (m_ShouldCreateMemberFunction)
			m_ShouldCreateMemberFunction = createNewMember();

		if (m_ShouldCreateFunction)
			m_ShouldCreateFunction = createNewNode();

		// Finally we can show the nodes.
		for (const auto& node : m_ActiveNodeBuilders)
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
		ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight, MiniMapHoveredCallback, m_ActiveNodeBuilders.data());

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

	bool NodeEditor::createNewClass()
	{
		ImGui::Begin("Create New Class");
		ImGui::TextWrapped("Classes are special nodes where there are no inputs, but is used to contain data. Every class gets its own 'this' member variable, which will be passed to member functions and will link member functions to classes.");
		ImGui::Separator();

		ImGui::ColorEdit3("Color", m_ColorPicker);
		ImGui::InputText("Namespace", m_NewNodeNamespaceBuffer, IM_ARRAYSIZE(m_NewNodeNamespaceBuffer));
		ImGui::InputText("Class name", m_NewNodeNameBuffer, IM_ARRAYSIZE(m_NewNodeNameBuffer));
		ImGui::Separator();

		// Get the variable names.
		ImGui::InputInt("Member variable count", &m_NewNodeOutputCount);
		if (m_NewNodeOutputCount < 0) m_NewNodeOutputCount = 0;

		m_NewNodeMemberNames.resize(m_NewNodeOutputCount);
		for (int32_t i = 0; i < m_NewNodeOutputCount; i++)
		{
			ImGui::InputText(("Member variable " + std::to_string(i + 1)).c_str(), m_NewNodeMemberNames[i].first.data(), m_NewNodeMemberNames[i].first.size());
			ImGui::SameLine(); if (ImGui::RadioButton(("public##" + std::to_string(i)).c_str(), m_NewNodeMemberNames[i].second == 0)) m_NewNodeMemberNames[i].second = 0;
			ImGui::SameLine(); if (ImGui::RadioButton(("private##" + std::to_string(i)).c_str(), m_NewNodeMemberNames[i].second == 1)) m_NewNodeMemberNames[i].second = 1;
			ImGui::SameLine(); if (ImGui::RadioButton(("protected##" + std::to_string(i)).c_str(), m_NewNodeMemberNames[i].second == 2)) m_NewNodeMemberNames[i].second = 2;
		}

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

			auto& node = m_ClassNodeBuilders.emplace_back(
				m_NewNodeNameBuffer,
				m_NodeID++,
				m_NodeAttributeID,
				NodeType::Class,
				IM_COL32(m_ColorPicker[0] * 256, m_ColorPicker[1] * 256, m_ColorPicker[2] * 256, 128),
				IM_COL32(m_ColorPicker[0] * 256, m_ColorPicker[1] * 256, m_ColorPicker[2] * 256, 192),
				IM_COL32(m_ColorPicker[0] * 256, m_ColorPicker[1] * 256, m_ColorPicker[2] * 256, 255));

			// Add the default this variable.
			node.addOutputAttribute("this", 1);

			// Create the output attributes.
			for (auto& name : m_NewNodeMemberNames)
				node.addOutputAttribute(name.first.data(), name.second);

			m_ActiveNodeBuilders.emplace_back(node);

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

	bool NodeEditor::createNewStruct()
	{
		ImGui::Begin("Create New Struct");
		ImGui::TextWrapped("Structs are much like classes, but the only difference is that in classes all the members are private by default. On structs they are public by default.");
		ImGui::Separator();

		ImGui::ColorEdit3("Color", m_ColorPicker);
		ImGui::InputText("Namespace", m_NewNodeNamespaceBuffer, IM_ARRAYSIZE(m_NewNodeNamespaceBuffer));
		ImGui::InputText("Struct name", m_NewNodeNameBuffer, IM_ARRAYSIZE(m_NewNodeNameBuffer));
		ImGui::Separator();

		// Get the variable names.
		ImGui::InputInt("Member variable count", &m_NewNodeOutputCount);
		if (m_NewNodeOutputCount < 0) m_NewNodeOutputCount = 0;

		m_NewNodeMemberNames.resize(m_NewNodeOutputCount);
		for (int32_t i = 0; i < m_NewNodeOutputCount; i++)
		{
			ImGui::InputText(("Member variable " + std::to_string(i + 1)).c_str(), m_NewNodeMemberNames[i].first.data(), m_NewNodeMemberNames[i].first.size());
			ImGui::SameLine(); if (ImGui::RadioButton(("public##" + std::to_string(i)).c_str(), m_NewNodeMemberNames[i].second == 0)) m_NewNodeMemberNames[i].second = 0;
			ImGui::SameLine(); if (ImGui::RadioButton(("private##" + std::to_string(i)).c_str(), m_NewNodeMemberNames[i].second == 1)) m_NewNodeMemberNames[i].second = 1;
			ImGui::SameLine(); if (ImGui::RadioButton(("protected##" + std::to_string(i)).c_str(), m_NewNodeMemberNames[i].second == 2)) m_NewNodeMemberNames[i].second = 2;
		}

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

			auto& node = m_StructNodeBuilders.emplace_back(
				m_NewNodeNameBuffer,
				m_NodeID++,
				m_NodeAttributeID,
				NodeType::Struct,
				IM_COL32(m_ColorPicker[0] * 256, m_ColorPicker[1] * 256, m_ColorPicker[2] * 256, 128),
				IM_COL32(m_ColorPicker[0] * 256, m_ColorPicker[1] * 256, m_ColorPicker[2] * 256, 255),
				IM_COL32(m_ColorPicker[0] * 256, m_ColorPicker[1] * 256, m_ColorPicker[2] * 256, 192));

			// Add the default this variable.
			node.addOutputAttribute("this", 1);

			// Create the output attributes.
			for (auto& name : m_NewNodeMemberNames)
				node.addOutputAttribute(name.first.data(), name.second);

			m_ActiveNodeBuilders.emplace_back(node);

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

	bool NodeEditor::createNewMember()
	{
		ImGui::Begin("Create New Member Function/ Node");
		ImGui::TextWrapped("Member functions/ nodes are special functions, which gets a default 'this' parameter, which will be passed to it by the linked class/ struct. These functions have access to the linked class's/ struct's private and protected members.");
		ImGui::Separator();

		ImGui::InputText("Namespace", m_NewNodeNamespaceBuffer, IM_ARRAYSIZE(m_NewNodeNamespaceBuffer));
		ImGui::InputText("Function name", m_NewNodeNameBuffer, IM_ARRAYSIZE(m_NewNodeNameBuffer));
		ImGui::Separator();

		// Get the input names.
		ImGui::InputInt("Input count", &m_NewNodeInputCount);
		if (m_NewNodeInputCount < 0) m_NewNodeInputCount = 0;

		m_NewNodeInputNames.resize(m_NewNodeInputCount);
		for (int32_t i = 0; i < m_NewNodeInputCount; i++)
			ImGui::InputText(("Input " + std::to_string(i + 1)).c_str(), m_NewNodeInputNames[i].data(), m_NewNodeInputNames[i].size());

		// Get the output names.
		ImGui::Separator();
		ImGui::InputInt("Output count", &m_NewNodeOutputCount);
		if (m_NewNodeOutputCount < 0) m_NewNodeOutputCount = 0;

		m_NewNodeOutputNames.resize(m_NewNodeOutputCount);
		for (int32_t i = 0; i < m_NewNodeOutputCount; i++)
			ImGui::InputText(("Output " + std::to_string(i + 1)).c_str(), m_NewNodeOutputNames[i].data(), m_NewNodeOutputNames[i].size());

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

			auto& node = m_MemberNodeBuilders.emplace_back(
				m_NewNodeNameBuffer,
				m_NodeID++,
				m_NodeAttributeID,
				NodeType::MemberFunction,
				IM_COL32(44, 117, 255, 128),
				IM_COL32(44, 117, 255, 255),
				IM_COL32(44, 117, 255, 196));

			// Add the default this variable.
			node.addInputAttribute("this");

			// Create the input attributes.
			for (auto& name : m_NewNodeInputNames)
				node.addInputAttribute(name.data());

			// Create the output attributes.
			for (auto& name : m_NewNodeOutputNames)
				node.addOutputAttribute(name.data());

			m_ActiveNodeBuilders.emplace_back(node);

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

	bool NodeEditor::createNewNode()
	{
		ImGui::Begin("Create New Function/ Node");
		ImGui::TextWrapped("This is a normal class, and is not bound to any class or struct.");
		ImGui::Separator();

		ImGui::InputText("Namespace", m_NewNodeNamespaceBuffer, IM_ARRAYSIZE(m_NewNodeNamespaceBuffer));
		ImGui::InputText("Node name", m_NewNodeNameBuffer, IM_ARRAYSIZE(m_NewNodeNameBuffer));
		ImGui::Separator();

		// Get the input names.
		ImGui::InputInt("Input count", &m_NewNodeInputCount);
		if (m_NewNodeInputCount < 0) m_NewNodeInputCount = 0;

		m_NewNodeInputNames.resize(m_NewNodeInputCount);
		for (int32_t i = 0; i < m_NewNodeInputCount; i++)
			ImGui::InputText(("Input " + std::to_string(i + 1)).c_str(), m_NewNodeInputNames[i].data(), m_NewNodeInputNames[i].size());

		// Get the output names.
		ImGui::Separator();
		ImGui::InputInt("Output count", &m_NewNodeOutputCount);
		if (m_NewNodeOutputCount < 0) m_NewNodeOutputCount = 0;

		m_NewNodeOutputNames.resize(m_NewNodeOutputCount);
		for (int32_t i = 0; i < m_NewNodeOutputCount; i++)
			ImGui::InputText(("Output " + std::to_string(i + 1)).c_str(), m_NewNodeOutputNames[i].data(), m_NewNodeOutputNames[i].size());

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

			auto& node = m_NodeBuilders.emplace_back(
				m_NewNodeNameBuffer,
				m_NodeID++,
				m_NodeAttributeID,
				NodeType::Function,
				IM_COL32(83, 69, 22, 128),
				IM_COL32(83, 69, 22, 255),
				IM_COL32(83, 69, 22, 196));

			// Create the input attributes.
			for (auto& name : m_NewNodeInputNames)
				node.addInputAttribute(name.data());

			// Create the output attributes.
			for (auto& name : m_NewNodeOutputNames)
				node.addOutputAttribute(name.data());

			m_ActiveNodeBuilders.emplace_back(node);

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
		std::fill_n(std::execution::unseq, m_NewNodeNamespaceBuffer, IM_ARRAYSIZE(m_NewNodeNamespaceBuffer), 0);

#else
		std::fill_n(m_NewNodeNameBuffer, IM_ARRAYSIZE(m_NewNodeNameBuffer), 0);
		std::fill_n(m_NewNodeNamespaceBuffer, IM_ARRAYSIZE(m_NewNodeNamespaceBuffer), 0);

#endif

		m_NewNodeInputCount = 0;
		m_NewNodeOutputCount = 0;
	}

	data_type NodeEditor::getDataType() const
	{
		if (ImGui::BeginCombo("Data Type", "void"))
		{
			ImGui::Selectable("int");
			ImGui::Selectable("float");
			ImGui::Selectable("double");
			ImGui::EndCombo();
		}

		return 0;
	}
}