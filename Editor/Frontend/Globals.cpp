// Copyright (c) 2022 Dhiraj Wishal

#include "Globals.hpp"

namespace rapid
{
	Globals& GetGlobals()
	{
		static Globals globals;
		return globals;
	}
}