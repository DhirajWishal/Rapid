// Copyright (c) 2022 Dhiraj Wishal

#pragma once

#include <chrono>

namespace rapid
{
	/**
	 * Limiter class.
	 * This object can be used to limit the application's framerate.
	 */
	class Limiter final
	{
		using clock = std::chrono::high_resolution_clock;
		using time_point = clock::time_point;
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param framerate The frame rate to limit.
		 */
		explicit Limiter(uint32_t framerate);

		/**
		 * Call this function once every iteration to keep the frame rate in check.
		 */
		void tick();

	private:
		const uint32_t m_FrameRateCap;
		time_point m_PreviousTime;
	};
}