// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <deque>
#include <vector>
#include <sstream>

namespace rapid
{
	/**
	 * File type enum.
	 */
	enum class FileType : uint8_t
	{
		Source,
		Header
	};

	/**
	 * Code generator class.
	 * This class is used to generate the C++ code.
	 *
	 * This uses a bunch of stacks to properly order the different stacks in the source/ header file.
	 */
	class CodeGenerator final
	{
		/**
		 * Stack structure.
		 */
		struct Stack final
		{
			std::stringstream m_SourceCode;
			//std::deque<Stack> m_Children;
		};

	public:
		/**
		 * Explicit constructor.
		 *
		 * @param type The Type of the file.
		 */
		explicit CodeGenerator(FileType type);

	private:
		std::vector<Stack> m_Stacks;

		const FileType m_Type;
	};
}