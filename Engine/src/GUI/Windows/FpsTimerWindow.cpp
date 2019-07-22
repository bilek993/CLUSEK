#include "FpsTimerWindow.h"
#include <imgui.h>
void FpsTimerWindow::Draw(const float deltaTime)
{
	Calculate(deltaTime);

	ImGui::Begin("FPS Timer", &IsEnabled);
	ImGui::Text("Frames per second: %i", ShowedFps);
	ImGui::End();
}

void FpsTimerWindow::Calculate(const float deltaTime)
{
	FramesCounter += 1;
	TimeCounter += deltaTime;

	if (TimeCounter > 1000.0f)
	{
		ShowedFps = FramesCounter;
		FramesCounter = 0;
		TimeCounter = 0.0f;
	}
}
