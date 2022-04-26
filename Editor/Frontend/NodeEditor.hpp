// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Components/Defaults.hpp"
#include "UIComponent.hpp"
#include <vector>
#include <array>

namespace rapid
{
	constexpr uint32_t MaximumStringLength = 256;

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
			 */
			explicit Attribute(std::string name, const int32_t ID) : m_AttributeName(name), m_AttributeID(ID) {}

			std::string m_AttributeName;
			const int32_t m_AttributeID;
		};

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param title The title of the node.
		 * @param nodeID The unique ID of the node.
		 */
		explicit NodeBuilder(std::string title, const int32_t nodeID, int32_t& attributeID);

		/**
		 * Add an input attribute to the node.
		 *
		 * @param name The name of the attribute.
		 */
		void addInputAttribute(std::string name);

		/**
		 * Add an output attribute to the node.
		 *
		 * @param name The name of the attribute.
		 */
		void addOutputAttribute(std::string name);

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
		 * Clone and create a new builder.
		 *
		 * @param nodeID The new node ID.
		 * @return The node builder.
		 */
		NodeBuilder clone(const int32_t nodeID) const;

	private:
		std::string m_Title;
		int32_t& m_AttributeID;
		const int32_t m_NodeID;

		std::vector<Attribute> m_InputAttributes;
		std::vector<Attribute> m_OutputAttributes;
	};

	/**
	 * Node editor class.
	 * This will contain the node editor logic.
	 */
	class NodeEditor final : public UIComponent
	{
	public:
		/**
		 * Default constructor.
		 */
		NodeEditor();

		/**
		 * Default destructor.
		 */
		~NodeEditor();

		/**
		 * Create a new node and return it's reference.
		 *
		 * @param title The node's title.
		 * @return The node builder reference.
		 */
		NodeBuilder& createNode(std::string title);

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
		 * Create a new class node.
		 * 
		 * @return Whether or not a new node was created.
		 */
		bool createNewClass();

		/**
		 * Create a new struct node.
		 *
		 * @return Whether or not a new node was created.
		 */
		bool createNewStruct();

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

	private:
		char m_NewNodeNameBuffer[MaximumStringLength] = "";
		int32_t m_NewNodeInputCount = 0;
		int32_t m_NewNodeOutputCount = 0;

		std::vector<NodeBuilder> m_ClassNodeBuilders;
		std::vector<NodeBuilder> m_StructNodeBuilders;
		std::vector<NodeBuilder> m_MemberNodeBuilders;
		std::vector<NodeBuilder> m_NodeBuilders;
		std::vector<NodeBuilder> m_ActiveNodeBuilders;

		std::vector<std::pair<int32_t, int32_t>> m_Links;
		std::vector<std::array<char, MaximumStringLength>> m_NewNodeInputNames;
		std::vector<std::array<char, MaximumStringLength>> m_NewNodeOutputNames;

		int32_t m_NodeID = 0;
		int32_t m_NodeAttributeID = 0;

		bool m_ShouldCreateClass = false;
		bool m_ShouldCreateStruct = false;
		bool m_ShouldCreateMemberFunction = false;
		bool m_ShouldCreateFunction = false;
	};
}