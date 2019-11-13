#pragma once
#include <chrono>

class Timer final
{
public:
	Timer();
	void Restart();
	float GetDeltaTimeAndRestart();
private:
	std::chrono::time_point<std::chrono::steady_clock> TimePoint;
};