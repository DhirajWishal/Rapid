// Copyright (c) 2022 Dhiraj Wishal

#include "FileExplorer.hpp"

#include <imgui.h>

namespace rapid
{
	FileExplorer::FileExplorer()
		: UIComponent("File Explorer"), m_SearchPath(std::filesystem::current_path())
	{
	}

	void FileExplorer::begin()
	{
		ImGui::Begin(m_Title.c_str());

		// Recursively get the directories and show them.
		showDirectory(std::filesystem::directory_entry(m_SearchPath));
	}

	void FileExplorer::end()
	{
		ImGui::End();
	}
	
	void FileExplorer::showDirectory(const std::filesystem::directory_entry& directory) const
	{
		for (const auto entry : std::filesystem::directory_iterator(directory))
		{
			const auto& string = entry.path().filename().string();
			if (entry.is_directory())
			{
				if (ImGui::CollapsingHeader(string.c_str()))
				{
					ImGui::Indent();
					showDirectory(entry);
					ImGui::Unindent();
				}
			}
			else
				ImGui::Text(string.c_str());
		}
	}
}