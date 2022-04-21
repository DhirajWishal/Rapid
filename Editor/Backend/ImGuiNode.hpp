// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Image.hpp"

namespace rapid
{
	/**
	 * ImGui node object.
	 * This node acts as a single processing unit in the rendering pipeline, and contains everything needed by ImGui to render to the screen.
	 */
	class ImGuiNode final : public BackendObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine object.
		 */
		explicit ImGuiNode(GraphicsEngine& engine);

		/**
		 * Destructor.
		 */
		~ImGuiNode();

		/**
		 * Terminate the node.
		 */
		void terminate() override;

	private:
		std::unique_ptr<Image> m_FontImage = nullptr;

		GraphicsEngine& m_Engine;
	};
}