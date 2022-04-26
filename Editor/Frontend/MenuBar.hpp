// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "UIComponent.hpp"

namespace rapid
{
	/**
	 * Menu bar class.
	 * This class contains all the info and commands regarding the menu bar.
	 */
	class MenuBar final : public UIComponent
	{
	public:
		/**
		 * Default constructor.
		 */
		MenuBar();

		/**
		 * Begin the stack.
		 */
		void begin() override;

		/**
		 * End the stack.
		 */
		void end() override;

	private:
		char m_ThemePath[256] = "";

		bool m_FileMenu = false;
		bool m_ViewMenu = false;

		bool m_FontItem = false;
		bool m_ThemeSelected = false;
	};
}