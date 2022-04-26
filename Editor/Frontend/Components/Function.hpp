// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Variable.hpp"

#include <vector>
#include <string>

namespace rapid
{
	/**
	 * Function class.
	 * This class is used to build a single function.
	 *
	 * Note that the data_type's value 0 is used to refer void.
	 */
	class Function final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param name The function name.
		 * @param type The return type.
		 */
		explicit Function(std::string&& name, data_type type) : m_Name(name), m_ReturnType(type) {}

		/**
		 * Add a parameter to the function.
		 *
		 * @param name The name of the parameter.
		 * @param type The parameter type.
		 * @param modifiers The parameter modifiers. In C++ this could be const, reference, [[maybe_unused]], etc...
		 */
		Function& addParameter(std::string&& name, data_type type, modifier_bits_type modifiers);

		/**
		 * Draw the function in node editor.
		 */
		void draw();

		/**
		 * Get the name of the function.
		 *
		 * @return The name string.
		 */
		std::string_view name() const { return m_Name; }

		/**
		 * Get the parameters.
		 *
		 * @return The parameters.
		 */
		std::vector<Variable>& parameters() { return m_Parameters; }

		/**
		 * Get the parameters.
		 *
		 * @return The parameters.
		 */
		const std::vector<Variable>& parameters() const { return m_Parameters; }

		/**
		 * Get the function's return type.
		 *
		 * @return The return type.
		 */
		data_type returnType() const { return m_ReturnType; }

	private:
		std::string m_Name;
		std::vector<Variable> m_Parameters;
		data_type m_ReturnType = 0;
	};
}