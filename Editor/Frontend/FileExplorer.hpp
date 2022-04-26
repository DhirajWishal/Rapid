// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "UIComponent.hpp"

#include <filesystem>

namespace rapid
{
	/**
	 * File explorer class.
	 * This is used to display all the contents in a directory.
	 */
	class FileExplorer final : public UIComponent
	{
	public:
		/**
		 * Default constructor.
		 */
		FileExplorer();

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
		 * Iterate through the directory recursively and display.
		 * 
		 * @param directory The directory to iterate and show.
		 */
		void showDirectory(const std::filesystem::directory_entry& directory) const;

	private:
		std::filesystem::path m_SearchPath;
	};
}