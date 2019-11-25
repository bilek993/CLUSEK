#pragma once
#include "BaseWindow.h"

class FpsTimerWindow final : public BaseWindow
{
protected:
	void Draw() override;
private:
	void Calculate();
	void AddNewFps(int fps);

	int FramesCounter = 0;
	float TimeCounter = 0.0f;
	float HistoricalFps[50] = {};
};
