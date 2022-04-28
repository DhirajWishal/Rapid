// Copyright (c) 2022 Dhiraj Wishal

#include "CodeGenerator.hpp"

namespace rapid
{
	CodeGenerator::CodeGenerator(FileType type)
		: m_Type(type)
	{
		// Let's have the global scope.
		auto& global = m_Stacks.emplace_back();

		// If it's a header file let's add the pragma notice.
		if (type == FileType::Header)
			global.m_SourceCode << "#pragma once" << std::endl;
	}
}