// Copyright (c) 2022 Dhiraj Wishal

#include "Limiter.hpp"

#include <thread>

namespace rapid
{
	Limiter::Limiter(uint32_t framerate)
		: m_FrameRateCap(framerate)
	{
	}

	void Limiter::tick()
	{
		// Calculate the current frame rate.
		auto newTime = clock::now();
		const auto diff = newTime - m_PreviousTime;
		const auto framerate = std::nano::den / diff.count();

		// Now we can sleep if the frame rate is higher than what we want.
		if (framerate > m_FrameRateCap)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1000ms / (framerate - m_FrameRateCap));
		}

		// Make sure to set this!
		m_PreviousTime = newTime;
	}
}