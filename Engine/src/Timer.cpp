#include "Timer.h"

Timer::Timer()
{
	TimePoint = std::chrono::high_resolution_clock::now();
}

float Timer::GetDeltaTimeAndRestart()
{
	auto elapsed = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - TimePoint);
	TimePoint = std::chrono::high_resolution_clock::now();
	return elapsed.count();
}
