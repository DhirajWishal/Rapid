// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "CommandBuffer.hpp"

namespace rapid
{
	class Window;

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
		 * @param window The window which owns the node.
		 */
		explicit ProcessingNode(GraphicsEngine& engine, Window& window) : m_Engine(engine), m_Window(window) {}

		/**
		 * Virtual destructor.
		 */
		virtual ~ProcessingNode() = default;

		/**
		 * This method will be called as soon as the new iteration starts.
		 *
		 * @param events The events of this iteration.
		 */
		virtual void onPollEvents(SDL_Event& events) = 0;

		/**
		 * Bind the resources to the command buffer.
		 *
		 * @param commandBuffer The command buffer to bind to.
		 * @param frameIndex The frame's index number.
		 */
		virtual void bind(CommandBuffer commandBuffer, uint32_t frameIndex) = 0;

	protected:
		GraphicsEngine& m_Engine;
		Window& m_Window;
	};

	/**
	 * Node type concept.
	 * This will check if the type is a base class of the ProcessingNode object.
	 */
	template<class Type>
	concept node_type = std::is_base_of_v<ProcessingNode, Type>;
}