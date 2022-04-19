// Copyright (c) 2022 Dhiraj Wishal

#include "UndoStack.hpp"

namespace rapid
{
	void UndoStack::issueCommand(UndoStack::callback_type undo, UndoStack::callback_type redo)
	{
		redo();
		m_UndoStack.push_front(std::make_pair(std::move(undo), std::move(redo)));

		// If we're over the threshold, let's pop back.
		if (m_UndoStack.size() > m_Threshold)
			m_UndoStack.pop_back();
	}

	void UndoStack::undo()
	{
		auto& pair = m_UndoStack.front();
		m_UndoStack.pop_front();

		pair.first();
		m_RedoStack.push_front(pair);

		// If we're over the threshold, let's pop back.
		if (m_RedoStack.size() > m_Threshold)
			m_RedoStack.pop_back();
	}

	void UndoStack::redo()
	{
		auto& pair = m_RedoStack.front();
		m_RedoStack.pop_front();

		pair.second();
		m_UndoStack.push_front(pair);

		// If we're over the threshold, let's pop back.
		if (m_UndoStack.size() > m_Threshold)
			m_UndoStack.pop_back();
	}
}
