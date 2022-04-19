// Copyright (c) 2022 Dhiraj Wishal

#define VMA_IMPLEMENTATION
#include "GraphicsEngine.hpp"

namespace rapid
{
	GraphicsEngine::GraphicsEngine()
	{
	}

	GraphicsEngine::~GraphicsEngine()
	{
		if (!m_IsTerminated)
			terminate();
	}

	void GraphicsEngine::terminate()
	{
		m_IsTerminated = true;
	}
}