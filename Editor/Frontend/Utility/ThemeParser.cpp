// Copyright (c) 2022 Dhiraj Wishal

#include "ThemeParser.hpp"
#include "../Console.hpp"

#include <imgui.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include <fstream>
#include <unordered_map>

namespace
{
	/**
	 * Load the file's content.
	 *
	 * @param path The file path.
	 * @return The file's content.
	 */
	std::string LoadFile(const std::filesystem::path& path)
	{
		std::fstream file(path, std::ios::in | std::ios::ate);

		// Validate the file.
		if (!file.is_open())
		{
			rapid::GetConsole().log("Failed to load the theme file!", rapid::Severity::Warning);
			return "";
		}

		// Get the file size.
		const auto size = file.tellg();
		file.seekg(0);

		// Create the buffer and load the data.
		std::string content;
		content.resize(size);
		file.read(content.data(), size);

		file.close();
		return content;
	}

	/**
	 * Convert the json array to a color.
	 *
	 * @param arr The array.
	 * @param value The color string.
	 * @param defaultColor The default color value.
	 * @return The color.
	 */
	ImVec4 GenericArrayToColor(const rapidjson::GenericArray<true, rapidjson::Value>& arr, std::string_view value, ImVec4 defaultColor)
	{
		if (arr.Size() != 4)
		{
			rapid::GetConsole().log(std::string("Invalid color type found in the theme file! Value: ") + value.data(), rapid::Severity::Warning);
			return defaultColor;
		}

		const float r = (arr[0].IsFloat() ? arr[0].GetFloat() : arr[0].GetInt()) / 256.0f;
		const float g = (arr[1].IsFloat() ? arr[1].GetFloat() : arr[1].GetInt()) / 256.0f;
		const float b = (arr[2].IsFloat() ? arr[2].GetFloat() : arr[2].GetInt()) / 256.0f;
		const float a = (arr[3].IsFloat() ? arr[3].GetFloat() : arr[3].GetInt()) / 256.0f;

		return ImVec4(r, g, b, a);
	}

	/**
	 * Static initializer structure.
	 */
	struct StaticInitializer final
	{
		/**
		 * Default constructor.
		 */
		StaticInitializer()
		{
			m_ColorMap["Text"] = ImGuiCol_Text;
			m_ColorMap["TextDisabled"] = ImGuiCol_TextDisabled;
			m_ColorMap["WindowBg"] = ImGuiCol_WindowBg;
			m_ColorMap["ChildBg"] = ImGuiCol_ChildBg;
			m_ColorMap["PopupBg"] = ImGuiCol_PopupBg;
			m_ColorMap["Border"] = ImGuiCol_Border;
			m_ColorMap["BorderShadow"] = ImGuiCol_BorderShadow;
			m_ColorMap["FrameBg"] = ImGuiCol_FrameBg;
			m_ColorMap["FrameBgHovered"] = ImGuiCol_FrameBgHovered;
			m_ColorMap["FrameBgActive"] = ImGuiCol_FrameBgActive;
			m_ColorMap["TitleBg"] = ImGuiCol_TitleBg;
			m_ColorMap["TitleBgActive"] = ImGuiCol_TitleBgActive;
			m_ColorMap["TitleBgCollapsed"] = ImGuiCol_TitleBgCollapsed;
			m_ColorMap["MenuBarBg"] = ImGuiCol_MenuBarBg;
			m_ColorMap["ScrollbarBg"] = ImGuiCol_ScrollbarBg;
			m_ColorMap["ScrollbarGrab"] = ImGuiCol_ScrollbarGrab;
			m_ColorMap["ScrollbarGrabHovered"] = ImGuiCol_ScrollbarGrabHovered;
			m_ColorMap["ScrollbarGrabActive"] = ImGuiCol_ScrollbarGrabActive;
			m_ColorMap["CheckMark"] = ImGuiCol_CheckMark;
			m_ColorMap["SliderGrab"] = ImGuiCol_SliderGrab;
			m_ColorMap["SliderGrabActive"] = ImGuiCol_SliderGrabActive;
			m_ColorMap["Button"] = ImGuiCol_Button;
			m_ColorMap["ButtonHovered"] = ImGuiCol_ButtonHovered;
			m_ColorMap["ButtonActive"] = ImGuiCol_ButtonActive;
			m_ColorMap["Header"] = ImGuiCol_Header;
			m_ColorMap["HeaderHovered"] = ImGuiCol_HeaderHovered;
			m_ColorMap["HeaderActive"] = ImGuiCol_HeaderActive;
			m_ColorMap["Separator"] = ImGuiCol_Separator;
			m_ColorMap["SeparatorHovered"] = ImGuiCol_SeparatorHovered;
			m_ColorMap["SeparatorActive"] = ImGuiCol_SeparatorActive;
			m_ColorMap["ResizeGrip"] = ImGuiCol_ResizeGrip;
			m_ColorMap["ResizeGripHovered"] = ImGuiCol_ResizeGripHovered;
			m_ColorMap["ResizeGripActive"] = ImGuiCol_ResizeGripActive;
			m_ColorMap["Tab"] = ImGuiCol_Tab;
			m_ColorMap["TabHovered"] = ImGuiCol_TabHovered;
			m_ColorMap["TabActive"] = ImGuiCol_TabActive;
			m_ColorMap["TabUnfocused"] = ImGuiCol_TabUnfocused;
			m_ColorMap["TabUnfocusedActive"] = ImGuiCol_TabUnfocusedActive;
			m_ColorMap["DockingPreview"] = ImGuiCol_DockingPreview;
			m_ColorMap["DockingEmptyBg"] = ImGuiCol_DockingEmptyBg;
			m_ColorMap["PlotLines"] = ImGuiCol_PlotLines;
			m_ColorMap["PlotLinesHovered"] = ImGuiCol_PlotLinesHovered;
			m_ColorMap["PlotHistogram"] = ImGuiCol_PlotHistogram;
			m_ColorMap["PlotHistogramHovered"] = ImGuiCol_PlotHistogramHovered;
			m_ColorMap["TableHeaderBg"] = ImGuiCol_TableHeaderBg;
			m_ColorMap["TableBorderStrong"] = ImGuiCol_TableBorderStrong;
			m_ColorMap["TableBorderLight"] = ImGuiCol_TableBorderLight;
			m_ColorMap["TableRowBg"] = ImGuiCol_TableRowBg;
			m_ColorMap["TableRowBgAlt"] = ImGuiCol_TableRowBgAlt;
			m_ColorMap["TextSelectedBg"] = ImGuiCol_TextSelectedBg;
			m_ColorMap["DragDropTarget"] = ImGuiCol_DragDropTarget;
			m_ColorMap["NavHighlight"] = ImGuiCol_NavHighlight;
			m_ColorMap["NavWindowingHighlight"] = ImGuiCol_NavWindowingHighlight;
			m_ColorMap["NavWindowingDimBg"] = ImGuiCol_NavWindowingDimBg;
			m_ColorMap["ModalWindowDimBg"] = ImGuiCol_ModalWindowDimBg;
		}

		/**
		 * Get the color from the container.
		 *
		 * @param color The color to get.
		 * @return The color.
		 */
		ImGuiCol getColor(std::string_view color) const { return m_ColorMap.at(color); }

	private:
		std::unordered_map<std::string_view, ImGuiCol> m_ColorMap;
	};

	/**
	 * Get the ImGui color from the color string.
	 *
	 * @param color The color to access.
	 * @return The color.
	 */
	ImGuiCol GetColor(std::string_view color)
	{
		static StaticInitializer initializer;
		return initializer.getColor(color);
	}

	/**
	 * Resolve the path from the incoming string.
	 * This basically identifies if the first two characters are "./" and if so, it appends the incoming json file's directory to it.
	 */
	std::filesystem::path ResolvePath(std::string_view path, std::filesystem::path&& directory)
	{
		// Check if we need the relative directory.
		if (path[0] == '.' && (path[1] == '/' || path[1] == '\\'))
			return directory / path.substr(2);

		return path;
	}
}

namespace rapid
{
	void LoadStyle(std::filesystem::path themeFile)
	{
		const auto content = LoadFile(themeFile);

		// Return if the content is empty.
		if (content.empty())
			return;

		// Parse the document.
		rapidjson::Document document;
		document.Parse(content.c_str());

		auto& styles = ImGui::GetStyle();

		// Iterate over the members and get the values.
		for (auto memberItr = document.MemberBegin(); memberItr != document.MemberEnd(); ++memberItr)
		{
			// Try and parse the colors.
			if (memberItr->name == "Colors")
			{
				const auto& colors = memberItr->value;
				for (auto itr = colors.MemberBegin(); itr != colors.MemberEnd(); ++itr)
				{
					const auto colorValue = GetColor(itr->name.GetString());
					const auto defaultColor = styles.Colors[colorValue];
					styles.Colors[colorValue] = itr->value.IsArray() ? GenericArrayToColor(itr->value.GetArray(), itr->name.GetString(), defaultColor) : defaultColor;
				}
			}

			// Try and parse alpha.
			else if (memberItr->name == "Alpha")
				styles.Alpha = memberItr->value.IsFloat() ? memberItr->value.GetFloat() : memberItr->value.IsInt() ? memberItr->value.GetInt() : styles.Alpha;

			// Try and parse alpha.
			else if (memberItr->name == "DisabledAlpha")
				styles.DisabledAlpha = memberItr->value.IsFloat() ? memberItr->value.GetFloat() : memberItr->value.IsInt() ? memberItr->value.GetInt() : styles.DisabledAlpha;

			// Try and parse font.
			else if (memberItr->name == "Font" && memberItr->value.Size() == 2)
			{
				const auto& fontData = memberItr->value;
				const auto fontFile = ResolvePath(fontData[0].GetString(), themeFile.parent_path());
				ImGui::GetIO().Fonts->AddFontFromFileTTF(fontFile.string().c_str(), fontData[1].GetFloat());
			}
		}
	}
}
