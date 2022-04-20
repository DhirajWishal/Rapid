// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "Function.hpp"

namespace rapid
{
	/**
	 * Access modifier enum.
	 */
	enum class AccessModifier : uint8_t
	{
		Public,
		Private,
		Protected
	};

	/**
	 * Object builder class.
	 * Objects could be any class type. Including
	 */
	class Object final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param name The name of the object.
		 * @param type The object type.
		 */
		explicit Object(std::string&& name, object_type type);

		/**
		 * Add a new member function to the object.
		 *
		 * @param function The function.
		 * @param modifier The access modifier.
		 */
		Object& addMethod(Function&& function, AccessModifier modifier);

		/**
		 * Create a new method.
		 *
		 * @param name The function name.
		 * @param type The method's return type.
		 * @param modifier The access modifier.
		 */
		Function& createMethod(std::string&& name, data_type type, AccessModifier modifier);

		/**
		 * Add a new member variable to the object.
		 *
		 * @param name The name of the variable.
		 * @param type The type of the variable.
		 * @param modifiers The modifiers of the variable.
		 * @param accessModifier The variable's access modifiers.
		 */
		Object& addVariable(std::string&& name, data_type type, modifier_bits_type modifiers, AccessModifier accessModifier);

		/**
		 * Add a new sub object to the object.
		 *
		 * @param object The object.
		 * @param modifier The access modifier.
		 */
		Object& addSubObject(Object&& object, AccessModifier modifier);

		/**
		 * Get the name of the object.
		 *
		 * @return The name.
		 */
		std::string_view name() const { return m_Name; }

		/**
		 * Get the member functions of the object.
		 *
		 * @return The methods with their access modifiers.
		 */
		std::vector<std::pair<Function, AccessModifier>>& methods() { return m_Functions; }

		/**
		 * Get the member functions of the object.
		 *
		 * @return The methods with their access modifiers.
		 */
		const std::vector<std::pair<Function, AccessModifier>>& methods() const { return m_Functions; }

		/**
		 * Get the member variables of the object.
		 *
		 * @return The variables with their access modifiers.
		 */
		std::vector<std::pair<Variable, AccessModifier>>& variables() { return m_Variables; }

		/**
		 * Get the member variables of the object.
		 *
		 * @return The variables with their access modifiers.
		 */
		const std::vector<std::pair<Variable, AccessModifier>>& variables() const { return m_Variables; }

		/**
		 * Get the sub objects of the object.
		 *
		 * @return The sub objects with their access modifiers.
		 */
		std::vector<std::pair<Object, AccessModifier>>& subObjects() { return m_SubObjects; }

		/**
		 * Get the sub objects of the object.
		 *
		 * @return The sub objects with their access modifiers.
		 */
		const std::vector<std::pair<Object, AccessModifier>>& subObjects() const { return m_SubObjects; }

		/**
		 * Get the type of the object.
		 *
		 * @return The type.
		 */
		object_type type() const { return m_Type; }

	private:
		std::string m_Name;
		std::vector<std::pair<Function, AccessModifier>> m_Functions;
		std::vector<std::pair<Variable, AccessModifier>> m_Variables;
		std::vector<std::pair<Object, AccessModifier>> m_SubObjects;
		object_type m_Type = 0;
	};
}