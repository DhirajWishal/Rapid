// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Components/Defaults.hpp"
#include "UIComponent.hpp"

#include <vector>
#include <array>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace rapid
{
	constexpr uint32_t MaximumStringLength = 32;

	/**
	 * Node type enum
	 */
	enum class NodeType : uint8_t
	{
		Class,
		Struct,
		MemberFunction,
		Function
	};

	/**
	 * Node builder class.
	 * This is used to build a new node.
	 *
	 * In order to show the node, call the show() method.
	 */
	class NodeBuilder final
	{
		/**
		 * Attribute structure.
		 */
		struct Attribute final
		{
			/**
			 * Explicit constructor.
			 *
			 * @param name The name of the attribute.
			 * @param ID The attribute ID.
			 * @param prop The property value. This is used by object types.
			 */
			explicit Attribute(std::string name, const int32_t ID, const int8_t prop) : m_AttributeName(name), m_AttributeID(ID), m_Property(prop) {}

			std::string m_AttributeName;
			const int32_t m_AttributeID;
			const int8_t m_Property;
		};

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param title The title of the node.
		 * @param nodeID The unique ID of the node.
		 * @param attributeID The attribute ID reference.
		 * @param type The node type.
		 * @param titleColor The title color.
		 * @param titleHoveredColor The title hovered color.
		 * @param titleSelectedColor The title selected color.
		 */
		explicit NodeBuilder(std::string title, const int32_t nodeID, int32_t& attributeID, NodeType type, uint32_t titleColor, uint32_t titleHoveredColor, uint32_t titleSelectedColor);

		/**
		 * Add an input attribute to the node.
		 *
		 * @param name The name of the attribute.
		 * @return The created attribute reference.
		 */
		Attribute& addInputAttribute(std::string name);

		/**
		 * Add an output attribute to the node.
		 *
		 * @param name The name of the attribute.
		 * @param prop The property value. This is used by object types. Default is -1.
		 * @return The created attribute reference.
		 */
		Attribute& addOutputAttribute(std::string name, int8_t prop = -1);

		/**
		 * Show the node to the user.
		 */
		void show() const;

		/**
		 * Get the node builder's title.
		 *
		 * @return The title.
		 */
		std::string_view getTitle() const { return m_Title; }

		/**
		 * Get the node ID.
		 *
		 * @return The ID.
		 */
		int32_t getID() const { return m_NodeID; }

		/**
		 * Get the node type.
		 *
		 * @return The node type.
		 */
		NodeType type() const { return m_Type; }

		/**
		 * Clone and create a new builder.
		 *
		 * @param nodeID The new node ID.
		 * @return The node builder.
		 */
		NodeBuilder clone(const int32_t nodeID) const;

		/**
		 * Get the attribute property.
		 *
		 * @param attribute The attribute ID.
		 * @return The property.
		 */
		int8_t getAttributeProperty(int32_t attribute) const;

		/**
		 * Get the input attributes.
		 *
		 * @return The attributes.
		 */
		const std::vector<Attribute>& getInputs() const { return m_InputAttributes; }

		/**
		 * Get the output attributes.
		 *
		 * @return The attributes.
		 */
		const std::vector<Attribute>& getOutputs() const { return m_OutputAttributes; }

	private:
		std::string m_Title;

		std::vector<Attribute> m_InputAttributes;
		std::vector<Attribute> m_OutputAttributes;

		int32_t& m_AttributeID;
		const int32_t m_NodeID;

		const uint32_t m_TitleColor;
		const uint32_t m_TitleHoveredColor;
		const uint32_t m_TitleSelectedColor;

		const NodeType m_Type;
	};

	/**
	 * Node editor class.
	 * This will contain the node editor logic.
	 */
	class NodeEditor final : public UIComponent
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param sourceFile The source file.
		 * @param headerFile The header file.
		 */
		explicit NodeEditor(std::filesystem::path&& sourceFile, std::filesystem::path&& headerFile);

		/**
		 * Default destructor.
		 */
		~NodeEditor();

		/**
		 * Begin the stack.
		 */
		void begin() override;

		/**
		 * End the stack.
		 */
		void end() override;

	private:
		/**
		 * Show class info function.
		 * This shows all the class information.
		 */
		void showClassInfo();

		/**
		 * Create a new member variable.
		 * 
		 * @return Whether or not a new member was created.
		 */
		bool createNewMemberVariable();

		/**
		 * Create a new member function node.
		 *
		 * @return Whether or not a new node was created.
		 */
		bool createNewMember();

		/**
		 * Create a new node.
		 *
		 * @return Whether or not a new node was created.
		 */
		bool createNewNode();

		/**
		 * Clean up the new node data.
		 */
		void cleanupNewNodeData();

		/**
		 * Show and get the data type from the user.
		 */
		data_type getDataType() const;

		/**
		 * Generate source code from the nodes.
		 */
		void generateSource() const;

	private:
		nlohmann::json m_JsonDocument;

		char m_NewNodeNameBuffer[MaximumStringLength] = "";
		char m_NewNodeNamespaceBuffer[MaximumStringLength] = "";
		float m_ColorPicker[3] = {};

		std::filesystem::path m_SourceFile;
		std::filesystem::path m_HeaderFile;

		std::vector<NodeBuilder> m_ClassNodeBuilders;
		std::vector<NodeBuilder> m_StructNodeBuilders;
		std::vector<NodeBuilder> m_MemberNodeBuilders;
		std::vector<NodeBuilder> m_NodeBuilders;

		std::vector<NodeBuilder> m_ActiveNodeBuilders;

		std::vector<std::pair<std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>>> m_Links;
		std::pair<std::array<char, MaximumStringLength>, int32_t> m_NewNodeMemberName;
		std::vector<std::array<char, MaximumStringLength>> m_NewNodeInputNames;
		std::vector<std::array<char, MaximumStringLength>> m_NewNodeOutputNames;

		int32_t m_NewNodeInputCount = 0;
		int32_t m_NewNodeOutputCount = 0;

		int32_t m_NodeID = 0;
		int32_t m_NodeAttributeID = 0;

		bool m_ShouldCreateMemberVariable = false;
		bool m_ShouldCreateMemberFunction = false;
		bool m_ShouldCreateFunction = false;
	};
}