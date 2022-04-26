// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <string>

namespace rapid
{
	/**
	 * UI Component object.
	 * This class is used to store a single UI component.
	 */
	class UIComponent
	{
	public:
		/**
		 * Explicit constructor.
		 */
		explicit UIComponent(std::string&& title) : m_Title(std::move(title)) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~UIComponent() = default;

		/**
		 * Begin the component's stack.
		 */
		virtual void begin() = 0;

		/**
		 * End the component's stack.
		 */
		virtual void end() = 0;

	protected:
		std::string m_Title;
	};
}