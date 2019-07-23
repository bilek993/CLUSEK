#pragma once

class FpsTimerWindow final
{
public:
	static void Draw(float deltaTime);

	inline static bool IsEnabled = false;
private:
	static void Calculate(float deltaTime);
	static void AddNewFps(int fps);

	inline static int FramesCounter = 0;
	inline static float TimeCounter = 0.0f;
	inline static float HistoricalFps[50];
};
