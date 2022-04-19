// Copyright (c) 2022 Dhiraj Wishal

#include "UndoStack.hpp"

namespace rapid
{
	void UndoStack::issueCommand(UndoStack::callback_type undo, UndoStack::callback_type redo)
	{
		redo();
		m_Stack.push(std::make_pair(std::move(undo), std::move(redo)));
	}
}
