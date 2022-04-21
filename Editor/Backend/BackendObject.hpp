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
		 * Explicit constructor.
		 *
		 * @param isTerminated Set the value of the internal boolean.
		 */
		explicit BackendObject(bool isTerminated) : m_IsTerminated(isTerminated) {}

		/**
		 * Default destructor.
		 */
		~BackendObject() = default;

		/**
		 * Pure virtual terminate method.
		 */
		virtual void terminate() = 0;

		/**
		 * Check if the object is active.
		 *
		 * @return Status.
		 */
		bool isActive() const { return !m_IsTerminated; }

	protected:
		bool m_IsTerminated = false;
	};
}