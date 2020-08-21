#include "Timer.h"

Timer::Timer()
{
	TimePoint = std::chrono::high_resolution_clock::now();
}

void Timer::Restart()
{
	TimePoint = std::chrono::high_resolution_clock::now();
}

float Timer::GetDelta() const
{
	const auto elapsed = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - TimePoint);
	return elapsed.count();
}

float Timer::GetDeltaTimeAndRestart()
{
	const auto elapsed = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - TimePoint);
	TimePoint = std::chrono::high_resolution_clock::now();
	return elapsed.count();
}
