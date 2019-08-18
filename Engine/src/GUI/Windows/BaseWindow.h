#pragma once
#include "../../Renderer/DynamicRenderSettings.h"
#include "../../Utils/SystemHolder.h"

class BaseWindow
{
public:
	virtual ~BaseWindow() = default;
	void Update(float deltaTime, ConfigData *configData, DynamicRenderSettings *renderSettings, std::vector<SystemHolder> *systems, IOData *ioData);
	bool& GetIsEnabled();

protected:
	virtual void Draw() = 0;

	bool IsEnabled = false;

	float DeltaTime = 0;
	ConfigData *Config = nullptr;
	DynamicRenderSettings *RenderSettings = nullptr;
	std::vector<SystemHolder> *Systems = nullptr;
	IOData *IoData = nullptr;
};
