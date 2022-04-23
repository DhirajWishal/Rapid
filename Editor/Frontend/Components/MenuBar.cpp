// Copyright (c) 2022 Dhiraj Wishal

#include "MenuBar.hpp"

#include <imgui.h>

namespace rapid
{
	MenuBar::MenuBar()
		: UIComponent("")
	{
	}

	void MenuBar::begin()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// Show the file menu.
			if (ImGui::BeginMenu("File", &m_FileMenu))
			{
				ImGui::EndMenu();
			}

			// Show the view menu.
			if (ImGui::BeginMenu("View", &m_ViewMenu))
			{
				// Show the font menu.
				if (ImGui::BeginMenu("Fonts", &m_FontItem))
				{
					auto& io = ImGui::GetIO();
					auto currentFont = ImGui::GetFont();
					for (int n = 0; n < io.Fonts->Fonts.Size; n++)
					{
						auto font = io.Fonts->Fonts[n];
						ImGui::PushID((void*)font);
						if (ImGui::Selectable(font->GetDebugName(), font == currentFont))
							io.FontDefault = font;
						ImGui::PopID();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
		}
	}

	void MenuBar::end()
	{
		ImGui::EndMainMenuBar();
	}
}