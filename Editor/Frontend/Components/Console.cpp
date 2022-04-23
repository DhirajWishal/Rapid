// Copyright (c) 2022 Dhiraj Wishal

#include "Console.hpp"

#include <imgui.h>
#include <ctime>

static std::time_t time_now = std::time(nullptr);

namespace
{
	/**
	 * Convert the message severity to a color.
	 *
	 * @param severity The message severity.
	 * @return The ImGui color.
	 */
	ImVec4 SeverityToColor(const rapid::Severity severity)
	{
		switch (severity)
		{
		case rapid::Severity::Info:			return ImVec4(0, 255, 0, 255);
		case rapid::Severity::Warning:		return ImVec4(255, 165, 0, 255);
		case rapid::Severity::Error:		return ImVec4(255, 0, 0, 255);
		}

		return ImVec4(255, 255, 255, 255);
	}
}

namespace rapid
{
	Console::Console()
		: UIComponent("Console")
	{
	}

	void Console::log(std::string&& message, Severity severity)
	{
		m_Messages.emplace_back(std::move(message), severity);
	}

	void Console::begin()
	{
		ImGui::Begin(m_Title.c_str());

		// Iterate over the messages and log them.
		for (const auto& message : m_Messages)
			ImGui::TextColored(SeverityToColor(message.m_Sevirity), message.m_Message.c_str());
	}

	void Console::end()
	{
		ImGui::End();
	}
	
	Console& GetConsole()
	{
		static Console console;
		return console;
	}
}