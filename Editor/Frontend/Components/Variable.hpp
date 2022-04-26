// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Defaults.hpp"
#include <string>

namespace rapid
{
	/**
	 * Variable structure.
	 * This contains information about a single parameter.
	 */
	struct Variable final
	{
		/**
		 * Explicit constructor.
		 *
		 * @param name The name of the parameter.
		 * @param type The parameter type.
		 * @param modifiers The parameter modifiers.
		 */
		explicit Variable(std::string&& name, data_type type, modifier_bits_type modifiers) : m_Name(std::move(name)), m_Type(type), m_Modifiers(modifiers) {}

		std::string m_Name;
		data_type m_Type;
		modifier_bits_type m_Modifiers;
	};
}