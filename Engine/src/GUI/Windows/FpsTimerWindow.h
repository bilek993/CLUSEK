#pragma once

class FpsTimerWindow final
{
public:
	inline static bool IsEnabled = false;
	static void Draw(float deltaTime);
private:
	inline static int FramesCounter = 0;
	inline static float TimeCounter = 0.0f;
	inline static int ShowedFps;
};
