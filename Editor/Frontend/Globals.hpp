// Copyright (c) 2022 Dhiraj Wishal

#pragma once

namespace rapid
{
	/**
	 * Globals structure.
	 * This contains all the global information used by the application.
	 */
	struct Globals final
	{
		bool m_ShouldRun = true;
	};

	/**
	 * Get the globals.
	 *
	 * @return The globals reference.
	 */
	Globals& GetGlobals();
}