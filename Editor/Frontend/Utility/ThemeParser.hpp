// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <filesystem>

namespace rapid
{
	/**
	 * Load a style from a source json file.
	 *
	 * @param themeFile The theme file.
	 */
	void LoadStyle(std::filesystem::path themeFile);
}