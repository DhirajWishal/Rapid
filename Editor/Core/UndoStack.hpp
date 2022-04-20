// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <cstdint>
#include <functional>
#include <deque>

namespace rapid
{
	/**
	 * Undo stack class.
	 * This class contains the basic undo and redo actions. Once a new undo-redo command is added, it will first run the
	 * redo command and then stores it.
	 * The undo and redo commands can be executed using the undo() and redo() functions.
	 *
	 * The container contains an internal threshold, and if the undo or the redo stack goes beyond them, the last
	 * command will get removed. Default is 10.
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
		 * Explicit constructor.
		 *
		 * @param threshold The undo and redo stack threshold.
		 */
		explicit UndoStack(uint8_t threshold) : m_Threshold(threshold) {}

		/**
		 * Issue a new undo and redo command to the stack.
		 *
		 * @param undo The undo command.
		 * @param redo The redo command.
		 */
		void issueCommand(callback_type undo, callback_type redo);

		/**
		 * Undo the last operation.
		 */
		void undo();

		/**
		 * Redo the last operation.
		 */
		void redo();

		/**
		 * Set the threshold value of the stack.
		 *
		 * @param threshold The threshold to set.
		 */
		void setThreshold(uint8_t threshold) { m_Threshold = threshold; }

		/**
		 * Get the threshold value.
		 *
		 * @return The threshold value.
		 */
		uint8_t threshold() const { return m_Threshold; }

	private:
		std::deque<std::pair<callback_type, callback_type>> m_UndoStack;
		std::deque<std::pair<callback_type, callback_type>> m_RedoStack;
		uint8_t m_Threshold = 10;
	};
}