// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <functional>
#include <stack>

namespace rapid
{
	/**
	 * Undo stack class.
	 * This class contains the basic undo and redo actions. Once a new undo-redo command is added, it will first run the
	 * redo command and then stores it.
	 * The undo and redo commands can be executed using the undo() and redo() functions.
	 */
	class UndoStack final
	{
	public:
		using callback_type = std::function<void()>;

		/**
		 * Default constructor.
		 */
		UndoStack() = default;

		/**
		 * Issue a new undo and redo command to the stack.
		 *
		 * @param undo The undo command.
		 * @param redo The redo command.
		 */
		void issueCommand(callback_type undo, callback_type redo);

	private:
		std::stack<std::pair<callback_type, callback_type>> m_Stack;
	};
}