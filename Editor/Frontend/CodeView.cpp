// Copyright (c) 2022 Dhiraj Wishal

#include "CodeView.hpp"

#include <imgui.h>

namespace rapid
{
	CodeView::CodeView()
		: UIComponent("Code View")
	{
	}

	void CodeView::setCode(const std::string& code)
	{
		m_Code = code;
	}

	void CodeView::begin()
	{
		ImGui::Begin(m_Title.c_str());

		// Temporary.
		ImGui::Text(m_Code.c_str());
	}

	void CodeView::end()
	{
		ImGui::End();
	}
}