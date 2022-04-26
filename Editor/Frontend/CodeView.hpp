// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "UIComponent.hpp"

namespace rapid
{
	/**
	 * Code view component.
	 * This is used to show a source code to the user.
	 */
	class CodeView final : public UIComponent
	{
	public:
		/**
		 * Default constructor.
		 */
		CodeView();

		/**
		 * Set the code to be shown.
		 *
		 * @param code The code to show.
		 */
		void setCode(const std::string& code);

		/**
		 * Begin the stack.
		 */
		void begin() override;

		/**
		 * End the stack.
		 */
		void end() override;

	private:
		std::string m_Code;
	};
}