#pragma once
#include <chrono>

class Timer final
{
public:
	Timer();
	float GetDeltaTimeAndRestart();
private:
	std::chrono::time_point<std::chrono::steady_clock> TimePoint;
};