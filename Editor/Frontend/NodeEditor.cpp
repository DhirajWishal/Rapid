// Copyright (c) 2022 Dhiraj Wishal

#include "NodeEditor.hpp"
#include "Console.hpp"

#include <imgui.h>
#include <imnodes.h>

#ifdef RAPID_PLATFORM_WINDOWS
#	include <execution>

#endif

#include <fstream>

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

	constexpr auto DefaultPublicColor = IM_COL32(0, 255, 0, 196);
	constexpr auto DefaultPublicColorHovered = IM_COL32(0, 255, 0, 255);

	constexpr auto DefaultPrivateColor = IM_COL32(255, 0, 0, 196);
	constexpr auto DefaultPrivateColorHovered = IM_COL32(255, 0, 0, 255);

	constexpr auto DefaultProtectedColor = IM_COL32(0, 0, 255, 196);
	constexpr auto DefaultProtectedColorHovered = IM_COL32(0, 0, 255, 255);
}

namespace rapid
{
	NodeBuilder::NodeBuilder(std::string title, const int32_t nodeID, int32_t& attributeID, NodeType type, uint32_t titleColor, uint32_t titleHoveredColor, uint32_t titleSelectedColor)
		: m_Title(std::move(title)), m_AttributeID(attributeID)
		, m_NodeID(nodeID), m_Type(type), m_TitleColor(titleColor)
		, m_TitleHoveredColor(titleHoveredColor), m_TitleSelectedColor(titleSelectedColor)
	{
	}

	NodeBuilder::Attribute& NodeBuilder::addInputAttribute(std::string name)
	{
		return m_InputAttributes.emplace_back(std::move(name), m_AttributeID++, -1);
	}

	NodeBuilder::Attribute& NodeBuilder::addOutputAttribute(std::string name, int8_t prop)
	{
		return m_OutputAttributes.emplace_back(std::move(name), m_AttributeID++, prop);
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
		int8_t previousProperty = -1;
		for (const auto& attribute : m_OutputAttributes)
		{
			// Push the property colors.
			if (attribute.m_Property == -1)
			{
				// Make sure to push the attribute flag so we can detach the link.
				ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

				ImNodes::PushColorStyle(ImNodesCol_Pin, ImNodes::GetStyle().Colors[ImNodesCol_Pin]);
				ImNodes::PushColorStyle(ImNodesCol_PinHovered, ImNodes::GetStyle().Colors[ImNodesCol_PinHovered]);

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
			else
			{
				ImNodes::BeginStaticAttribute(attribute.m_AttributeID);

				if (previousProperty != attribute.m_Property)
				{
					previousProperty = attribute.m_Property;

					if (attribute.m_Property == 0)
						ImGui::Text("public:");

					else if (attribute.m_Property == 1)
						ImGui::Text("private:");

					else if (attribute.m_Property == 2)
						ImGui::Text("protected:");
				}

				ImGui::Text(("\t" + attribute.m_AttributeName).data());
				ImNodes::EndStaticAttribute();
			}
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

	int8_t NodeBuilder::getAttributeProperty(int32_t attribute) const
	{
		for (const auto& outputAttribute : m_OutputAttributes)
		{
			if (outputAttribute.m_AttributeID == attribute)
				return outputAttribute.m_Property;
		}

		return -1;
	}

	NodeEditor::NodeEditor(std::filesystem::path&& sourceFile, std::filesystem::path&& headerFile)
		: UIComponent(sourceFile.string().empty() ? headerFile.stem().string() : sourceFile.stem().string())
		, m_SourceFile(std::move(sourceFile)), m_HeaderFile(std::move(headerFile))
	{
		ImNodes::CreateContext();

		// Try and get the previous state.
		ImNodes::LoadCurrentEditorStateFromIniFile("editor.ini");

		std::ifstream jsonFile(m_Title + ".rof");
		if (jsonFile.is_open())
		{
			jsonFile >> m_JsonDocument;
		}
		else
		{
			m_JsonDocument["name"] = m_Title;
		}
		jsonFile.close();
	}

	NodeEditor::~NodeEditor()
	{
		// Generate the source.
		generateSource();

		// Save and destroy the context.
		ImNodes::SaveCurrentEditorStateToIniFile("editor.ini");
		ImNodes::DestroyContext();
	}

	void NodeEditor::begin()
	{
		// Begin the editor.
		ImGui::Begin(m_Title.c_str());
		ImNodes::BeginNodeEditor();

		const auto& jsonNode = m_JsonDocument["name"];

		for (const auto& node : jsonNode)
		{
		}

		// If the user right-clicks, let's show the menu.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
		if (ImGui::BeginPopupContextWindow("Options", ImGuiPopupFlags_MouseButtonRight))
		{
			// Member variables.
			if (ImGui::Selectable("Create member variable")) m_ShouldCreateMemberVariable = true;
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Create a new member variable.");

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

		// If we should, let's create a new node(s).
		if (m_ShouldCreateMemberVariable)
			m_ShouldCreateMemberVariable = createNewMemberVariable();

		if (m_ShouldCreateMemberFunction)
			m_ShouldCreateMemberFunction = createNewMember();

		if (m_ShouldCreateFunction)
			m_ShouldCreateFunction = createNewNode();

		ImGui::PopStyleVar();

		// Finally we can show the nodes.
		for (const auto& node : m_ActiveNodeBuilders)
			node.show();
	}

	void NodeEditor::end()
	{
		// Create the links if we have any.
		for (int i = 0; i < m_Links.size(); ++i)
		{
			const auto p = m_Links[i];
			bool shouldPop = true;

			for (const auto& node : m_ActiveNodeBuilders)
			{
				if (node.getID() == p.first.first || node.getID() == p.second.first)
				{
					auto prop = node.getAttributeProperty(p.first.second);
					if (prop < 0)
						prop = node.getAttributeProperty(p.second.second);

					if (prop == -1)
					{
						shouldPop = false;
					}
					else if (prop == 0)
					{
						ImNodes::PushColorStyle(ImNodesCol_Link, DefaultPublicColor);
						ImNodes::PushColorStyle(ImNodesCol_LinkHovered, DefaultPublicColorHovered);
						ImNodes::PushColorStyle(ImNodesCol_LinkSelected, DefaultPublicColorHovered);
					}
					else if (prop == 1)
					{
						ImNodes::PushColorStyle(ImNodesCol_Link, DefaultPrivateColor);
						ImNodes::PushColorStyle(ImNodesCol_LinkHovered, DefaultPrivateColorHovered);
						ImNodes::PushColorStyle(ImNodesCol_LinkSelected, DefaultPrivateColorHovered);
					}
					else if (prop == 2)
					{
						ImNodes::PushColorStyle(ImNodesCol_Link, DefaultProtectedColor);
						ImNodes::PushColorStyle(ImNodesCol_LinkHovered, DefaultProtectedColorHovered);
						ImNodes::PushColorStyle(ImNodesCol_LinkSelected, DefaultProtectedColorHovered);
					}

					break;
				}
			}

			ImNodes::Link(i, p.first.second, p.second.second);

			if (shouldPop)
			{
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
				ImNodes::PopColorStyle();
			}
		}

		// Make sure to show the mini map before we end!
		ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight, MiniMapHoveredCallback, m_ActiveNodeBuilders.data());

		//ImNodes::PopAttributeFlag();
		ImNodes::EndNodeEditor();
		ImGui::End();

		// Resolve the links.
		int32_t startNode, startLink, endNode, endLink;
		if (ImNodes::IsLinkCreated(&startNode, &startLink, &endNode, &endLink))
			m_Links.emplace_back(std::make_pair(startNode, startLink), std::make_pair(endNode, endLink));

		// Remove links if we have to.
		int32_t link;
		if (ImNodes::IsLinkDestroyed(&link))
			m_Links.erase(m_Links.begin() + link);

		// Show what the link is when hovered. TODO
		int32_t hoveredLink;
		if (ImNodes::IsLinkHovered(&hoveredLink))
			ImGui::SetTooltip("Link");

		// Show the class information.
		showClassInfo();
	}

	void NodeEditor::showClassInfo()
	{
		ImGui::Begin("Class Information");
		ImGui::Text("Class name: %s", m_Title.c_str());

		ImGui::Separator();
		ImGui::Text("Member variables");

		if (m_JsonDocument["name"].size() > 1)
		{
			int8_t previousProperty = -1;
			auto& node = m_JsonDocument["name"]["variables"];
			for (auto itr = node.begin(); itr != node.end(); ++itr)
			{
				const auto& variable = *itr;
				const auto& name = itr.key();
				const auto& type = variable["type"].get<std::string>();
				const auto prop = variable["access"].get<int>();

				if (previousProperty != prop)
				{
					previousProperty = prop;

					if (prop == 0)
						ImGui::Text("public:");

					else if (prop == 1)
						ImGui::Text("private:");

					else if (prop == 2)
						ImGui::Text("protected:");
				}

				ImGui::Text(("\t" + type + " " + name).c_str());
			}
		}

		ImGui::End();
	}

	bool NodeEditor::createNewMemberVariable()
	{
		ImGui::Begin("Create New Member Variable");

		// Get the variable names.
		ImGui::InputText("Member variable", m_NewNodeMemberName.first.data(), m_NewNodeMemberName.first.size());
		if (ImGui::RadioButton("public", m_NewNodeMemberName.second == 0)) m_NewNodeMemberName.second = 0;
		ImGui::SameLine(); if (ImGui::RadioButton("private", m_NewNodeMemberName.second == 1)) m_NewNodeMemberName.second = 1;
		ImGui::SameLine(); if (ImGui::RadioButton("protected", m_NewNodeMemberName.second == 2)) m_NewNodeMemberName.second = 2;

		// We can try to create if the user wants to.
		if (ImGui::Button("Create"))
		{
			ImGui::End();

			// Check if we have data, if not we can return.
			if (!m_NewNodeMemberName.first[0])
			{
				GetConsole().log("Failed to create a new node! Make sure that you have data to create the node first.", Severity::Warning);
				return true;
			}

			auto& node = m_JsonDocument["name"]["variables"][m_NewNodeMemberName.first.data()];
			node["access"] = m_NewNodeMemberName.second;
			node["type"] = "int";

			// Make sure to clear the values before we leave!
			m_NewNodeMemberName.first.fill(0);
			return false;
		}

		// Cancel if the user wants to.
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			m_NewNodeMemberName.first.fill(0);

			ImGui::End();
			return false;
		}

		// Or we can clear.
		ImGui::SameLine();
		if (ImGui::Button("Clear")) m_NewNodeMemberName.first.fill(0);;

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
				IM_COL32(10, 80, 192, 255),
				IM_COL32(0, 98, 255, 255),
				IM_COL32(22, 93, 207, 255));

			// Add the default this variable.
			node.addInputAttribute("this");

			auto& jsonNode = m_JsonDocument["name"]["MemberFunctions"][node.getTitle()];

			// Create the input attributes.
			for (auto& name : m_NewNodeInputNames)
			{
				node.addInputAttribute(name.data());
				jsonNode["parameters"][name.data()] = "int";
			}

			// Create the output attributes.
			for (auto& name : m_NewNodeOutputNames)
			{
				node.addOutputAttribute(name.data());
				jsonNode["returns"][name.data()] = "int";
			}

			jsonNode["ID"] = m_NodeID++;
			jsonNode["normal"] = IM_COL32(m_ColorPicker[0] * 255, m_ColorPicker[1] * 255, m_ColorPicker[2] * 255, 128);
			jsonNode["hovered"] = IM_COL32(m_ColorPicker[0] * 255, m_ColorPicker[1] * 255, m_ColorPicker[2] * 255, 192);
			jsonNode["selected"] = IM_COL32(m_ColorPicker[0] * 255, m_ColorPicker[1] * 255, m_ColorPicker[2] * 255, 255);

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
				IM_COL32(93, 41, 178, 255),
				IM_COL32(147, 81, 255, 255),
				IM_COL32(133, 54, 209, 255)
			);

			auto& jsonNode = m_JsonDocument["name"]["Functions"][node.getTitle()];

			// Create the input attributes.
			for (auto& name : m_NewNodeInputNames)
			{
				node.addInputAttribute(name.data());
				jsonNode["parameters"][name.data()] = "int";
			}

			// Create the output attributes.
			for (auto& name : m_NewNodeOutputNames)
			{
				node.addOutputAttribute(name.data());
				jsonNode["returns"][name.data()] = "int";
			}

			jsonNode["ID"] = m_NodeID++;
			jsonNode["normal"] = IM_COL32(m_ColorPicker[0] * 255, m_ColorPicker[1] * 255, m_ColorPicker[2] * 255, 128);
			jsonNode["hovered"] = IM_COL32(m_ColorPicker[0] * 255, m_ColorPicker[1] * 255, m_ColorPicker[2] * 255, 192);
			jsonNode["selected"] = IM_COL32(m_ColorPicker[0] * 255, m_ColorPicker[1] * 255, m_ColorPicker[2] * 255, 255);

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

	void NodeEditor::generateSource() const
	{
		std::ofstream sourceFile("SampleSource.cpp");
		if (sourceFile.is_open())
		{
			// Define classes.
			for (const auto& node : m_ClassNodeBuilders)
			{
				sourceFile << "class " << node.getTitle() << " final" << std::endl;
				sourceFile << "{" << std::endl;

				int8_t oldProperty = -1;
				// Write all the output attributes.
				for (const auto& attribute : node.getOutputs())
				{
					if (attribute.m_AttributeName == "this") continue;

					if (attribute.m_Property == -1)
						sourceFile << "int m_" << attribute.m_AttributeName << ";" << std::endl;

					else if (attribute.m_Property == 0 && oldProperty != 0)
					{
						sourceFile << "public:" << std::endl;
						oldProperty = 0;
					}
					else if (attribute.m_Property == 1 && oldProperty != 1)
					{
						sourceFile << "private:" << std::endl;
						oldProperty = 1;
					}
					else if (attribute.m_Property == 2 && oldProperty != 2)
					{
						sourceFile << "protected:" << std::endl;
						oldProperty = 2;
					}

					sourceFile << "int m_" << attribute.m_AttributeName << ";" << std::endl;
				}

				sourceFile << "};" << std::endl << std::endl;
			}

			// Define structs.
			for (const auto& node : m_StructNodeBuilders)
			{
				sourceFile << "struct " << node.getTitle() << " final" << std::endl;
				sourceFile << "{" << std::endl;

				int8_t oldProperty = -1;
				// Write all the output attributes.
				for (const auto& attribute : node.getOutputs())
				{
					if (attribute.m_AttributeName == "this") continue;

					if (attribute.m_Property == -1)
						sourceFile << "int m_" << attribute.m_AttributeName << ";" << std::endl;

					else if (attribute.m_Property == 0 && oldProperty != 0)
					{
						sourceFile << "public:" << std::endl;
						oldProperty = 0;
					}
					else if (attribute.m_Property == 1 && oldProperty != 1)
					{
						sourceFile << "private:" << std::endl;
						oldProperty = 1;
					}
					else if (attribute.m_Property == 2 && oldProperty != 2)
					{
						sourceFile << "protected:" << std::endl;
						oldProperty = 2;
					}

					sourceFile << "int m_" << attribute.m_AttributeName << ";" << std::endl;
				}

				sourceFile << "};" << std::endl << std::endl;
			}

			// Define functions.
			for (const auto& node : m_NodeBuilders)
			{
				sourceFile << "decltype(auto) " << node.getTitle() << "(";

				// Write all the input attributes.
				const auto& inputs = node.getInputs();
				for (uint32_t i = 0; i < inputs.size(); ++i)
				{
					const auto& attribute = inputs[i];
					sourceFile << "int " << attribute.m_AttributeName;

					if (i < (inputs.size() - 1))
						sourceFile << ", ";
				}

				sourceFile << ")" << std::endl << "{" << std::endl;
				sourceFile << "return std::make_tuple(";

				// Write all the output attributes.
				const auto& outputs = node.getOutputs();
				for (uint32_t i = 0; i < outputs.size(); ++i)
				{
					const auto& attribute = outputs[i];
					sourceFile << attribute.m_AttributeName;

					if (i < (outputs.size() - 1))
						sourceFile << ", ";
				}

				sourceFile << ");" << std::endl << "}" << std::endl << std::endl;
			}

			// Now we can link the two using the links.
			for (const auto& link : m_Links)
			{
				// const auto& fromNode = m_ActiveNodeBuilders[link.first.first];
				// const auto& toNode = m_ActiveNodeBuilders[link.second.first];

				// The linking logic.
			}
		}

		sourceFile.close();

		std::ofstream jsonFile(m_Title + ".rof");
		jsonFile << m_JsonDocument.dump(0);
		jsonFile.close();
	}
}