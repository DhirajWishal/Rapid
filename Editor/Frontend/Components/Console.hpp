// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include "UIComponent.hpp"

#include <vector>

namespace rapid
{
	/**
	 * Message severity.
	 */
	enum class Severity : uint8_t
	{
		Info,
		Warning,
		Error
	};

	/**
	 * Console class.
	 * This can be used to log messages to the user.
	 */
	class Console final : public UIComponent
	{
		/**
		 * Message structure.
		 */
		struct Message final
		{
			/**
			 * Explicit constructor.
			 *
			 * @param message The message to log.
			 * @param severity The message severity.
			 */
			explicit Message(std::string&& message, Severity severity) : m_Message(std::move(message)), m_Sevirity(severity) {}

			const std::string m_Message;
			const Severity m_Sevirity;
		};

	public:
		/**
		 * Default constructor.
		 */
		Console();

		/**
		 * Log a new message.
		 *
		 * @param message The message to log.
		 * @param severity The message severity.
		 */
		void log(std::string&& message, Severity severity);

		/**
		 * Begin the stack.
		 */
		void begin() override;

		/**
		 * End the stack.
		 */
		void end() override;

	private:
		std::vector<Message> m_Messages;
	};

	/**
	 * Get the global console.
	 *
	 * @return The console.
	 */
	Console& GetConsole();
}