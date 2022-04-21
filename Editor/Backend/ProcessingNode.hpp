// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "GraphicsEngine.hpp"

namespace rapid
{
	/**
	 * Processing node.
	 * Processing nodes are used to render a set of objects to the screen. In this application, we use nodes to render the UI.
	 */
	class ProcessingNode : public BackendObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which the object is bound to.
		 */
		explicit ProcessingNode(GraphicsEngine& engine) : m_Engine(engine) {}

		/**
		 * Bind the resources to the command buffer.
		 *
		 * @param vCommandBuffer The command buffer to bind to.
		 */
		virtual void bind(VkCommandBuffer vCommandBuffer) = 0;

	protected:
		GraphicsEngine& m_Engine;
	};
}