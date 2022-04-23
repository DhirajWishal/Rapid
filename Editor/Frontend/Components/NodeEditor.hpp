// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "UIComponent.hpp"
#include <vector>

namespace rapid
{
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
			explicit Attribute(std::string_view name, const int32_t ID) : m_AttributeName(name), m_AttributeID(ID) {}

			std::string_view m_AttributeName;
			const int32_t m_AttributeID;
		};

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param title The title of the node.
		 * @param nodeID The unique ID of the node.
		 */
		explicit NodeBuilder(std::string_view title, const int32_t nodeID);

		/**
		 * Add an input attribute to the node.
		 *
		 * @param name The name of the attribute.
		 * @param ID The attribute ID.
		 */
		void addInputAttribute(std::string_view name, const int32_t ID);

		/**
		 * Add an output attribute to the node.
		 *
		 * @param name The name of the attribute.
		 * @param ID The attribute ID.
		 */
		void addOutputAttribute(std::string_view name, const int32_t ID);

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

	private:
		std::string_view m_Title;
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
		NodeBuilder& createNode(std::string_view title);

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
		 * Create a new node.
		 * 
		 * @return Whether or not a new node was created.
		 */
		bool CreateNewNode();

	private:
		char m_NewNodeNameBuffer[256] = "";

		std::vector<std::pair<int32_t, int32_t>> m_Links;
		std::vector<rapid::NodeBuilder> m_NodeBuilders;

		int32_t m_NodeID = 0;
	};
}