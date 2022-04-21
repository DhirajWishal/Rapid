// Copyright (c) 2022 Dhiraj Wishal

#pragma once

namespace rapid
{
	/**
	 * Backend Object class.
	 * This object is the super class for all the backend objects.
	 */
	class BackendObject
	{
	public:
		/**
		 * Default constructor.
		 */
		BackendObject() = default;

		/**
		 * Default destructor.
		 */
		~BackendObject() = default;

		/**
		 * Pure virtual terminate method.
		 */
		virtual void terminate() = 0;

	protected:
		bool m_IsTerminated = false;
	};
}