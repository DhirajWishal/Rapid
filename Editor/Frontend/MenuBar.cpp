// Copyright (c) 2022 Dhiraj Wishal

#include "MenuBar.hpp"
#include "Utility/ThemeParser.hpp"
#include "Utility/CloseEvent.hpp"

#include <imgui.h>
#include <SDL.h>

#ifdef RAPID_PLATFORM_WINDOWS
#	include <execution>

#endif

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

				// Show the load theme option.
				if (ImGui::MenuItem("Themes"))
					m_ThemeSelected = true;

				ImGui::EndMenu();
			}
		}

		// Show the theme selector if selected.
		if (m_ThemeSelected)
		{
			ImGui::Begin("Theme selector");
			ImGui::InputText("Path to .json", m_ThemePath, IM_ARRAYSIZE(m_ThemePath));

			// If the user selects apply, we can load and apply the theme.
			if (ImGui::Button("Apply"))
			{
				// Load the style if possible.
				LoadStyle(m_ThemePath);

				// Clear the array.
#ifdef RAPID_PLATFORM_WINDOWS
				std::fill_n(std::execution::unseq, m_ThemePath, IM_ARRAYSIZE(m_ThemePath), 0);

#else
				std::fill_n(m_ThemePath, IM_ARRAYSIZE(m_ThemePath), 0);

#endif

				m_ThemeSelected = false;
			}

			ImGui::SameLine();
			// If the user cancels it, we can leave.
			if (ImGui::Button("Cancel")) m_ThemeSelected = false;

			ImGui::End();
		}
	}

	void MenuBar::end()
	{
		ImGui::EndMainMenuBar();
	}
}