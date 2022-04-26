// Copyright (c) 2022 Dhiraj Wishal

#include "Function.hpp"

namespace rapid
{
	Function& Function::addParameter(std::string&& name, data_type type, modifier_bits_type modifiers)
	{
		m_Parameters.emplace_back(std::move(name), type, modifiers);
		return *this;
	}
	
	void Function::draw()
	{
	}
}