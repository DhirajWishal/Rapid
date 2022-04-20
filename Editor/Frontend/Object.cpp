// Copyright (c) 2022 Dhiraj Wishal

#include "Object.hpp"

namespace rapid
{
	Object& Object::addMethod(Function&& function, AccessModifier modifier)
	{
		m_Functions.emplace_back(std::move(function), modifier);
		return *this;
	}

	Function& Object::createMethod(std::string&& name, data_type type, AccessModifier modifier)
	{
		return m_Functions.emplace_back(Function(std::move(name), type), modifier).first;
	}

	Object& Object::addVariable(std::string&& name, data_type type, modifier_bits_type modifiers, AccessModifier accessModifier)
	{
		m_Variables.emplace_back(Variable(std::move(name), type, modifiers), accessModifier);
		return *this;
	}

	Object& Object::addSubObject(Object&& object, AccessModifier modifier)
	{
		m_SubObjects.emplace_back(std::move(object), modifier);
		return *this;
	}
}