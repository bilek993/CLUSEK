#pragma once
#include "../../Renderer/DynamicRenderSettings.h"
#include "../../Utils/SystemHolder.h"

class BaseWindow
{
public:
	virtual ~BaseWindow() = default;
	void Update(float deltaTime, DynamicRenderSettings *renderSettings, std::vector<SystemHolder> *systems);
	bool& GetIsEnabled();

protected:
	virtual void Draw() = 0;

	bool IsEnabled = false;

	float DeltaTime = 0;
	DynamicRenderSettings *RenderSettings = nullptr;
	std::vector<SystemHolder> *Systems = nullptr;
};
