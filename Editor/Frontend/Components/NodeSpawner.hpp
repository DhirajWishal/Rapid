// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "NodeEditor.hpp"

namespace rapid
{
	/**
	 * Node spawner class.
	 * This has two functionalities, both to spawn nodes and show the available/ created nodes.
	 */
	class NodeSpawner final : public UIComponent
	{
	public:
		/**
		 * Default constructor.
		 */
		NodeSpawner();

		

		/**
		 * Show all the nodes.
		 * Make sure that you call this method within the node editor's begin and end.
		 */
		void showNodes() const;

		/**
		 * Begin the stack.
		 */
		void begin() override;

		/**
		 * End the stack.
		 */
		void end() override;

	private:
		std::vector<rapid::NodeBuilder> m_NodeBuilders;

		int32_t m_NodeID = 0;
		int32_t m_NodeInputID = 0;
		int32_t m_NodeOutputID = 0;
	};
}