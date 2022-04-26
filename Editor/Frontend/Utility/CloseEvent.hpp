// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <cstdint>

namespace rapid
{
	/**
	 * Close event class.
	 * This class is thrown by the application if it is needed to be closed.
	 */
	class CloseEvent final
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param status The exit status. Default is 0.
		 */
		explicit CloseEvent(int32_t status = 0) : m_Status(status) {}

		/**
		 * Get the status.
		 *
		 * @return The exit status.
		 */
		int32_t status() const { return m_Status; }

	private:
		const int32_t m_Status;
	};
}