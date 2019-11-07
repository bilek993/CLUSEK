#pragma once
#include <vector>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "../Utils/SystemHolder.h"
#include <d3d11.h>
#include "Windows/BaseWindow.h"
#include "Windows/SystemsManagerWindow.h"
#include "Windows/FpsTimerWindow.h"
#include "Windows/LightingWindow.h"
#include "Windows/BackBufferWindow.h"
#include "Windows/KeyboardInputWindow.h"
#include "Windows/MouseInputWindow.h"
#include "Windows/GamePadWindow.h"
#include "Windows/CameraSpeedWindow.h"
#include "Windows/PostProcessingWindow.h"
#include "Windows/VehicleDetailsWindow.h"

class DebugUserInterface final
{
public:
	void Initialize(const HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ConfigData *configData,
		const std::function<void()> &functionCloseEngine);
	void Update(float deltaTime, ConfigData *configData, IOData *ioData, 
		std::vector<SystemHolder> *systems, int renderSystemId, DynamicRenderSettings *dynamicRenderSettings, PostProcessingSettings *postProcessingSettings);

	~DebugUserInterface();
private:
	void SetupStyle(const ConfigData *configData) const;

	void HandleKeyboardEvents(const IOData *ioData);
	void HandleMainDockingArea();

	void BeforeUpdate() const;
	void AfterUpdate() const;

	void DrawMenuBar();

	inline const static std::string DOCKSPACE_AREA_NAME = "MainDockspaceArea";

	bool IsEnabled = false;
	bool IsDockingEnabled = false;

	std::function<void()> FunctionCloseEngine;

	CameraSpeedWindow CameraSpeedWindowInstance;
	SystemsManagerWindow SystemsManagerWindowInstance;
	FpsTimerWindow FpsTimerWindowInstance;
	LightingWindow LightingWindowInstance;
	BackBufferWindow BackBufferWindowInstance;
	MouseInputWindow MouseInputWindowInstance;
	KeyboardInputWindow KeyboardInputWindowInstance;
	GamePadInputWindow GamePadInputWindowInstance;
	PostProcessingWindow PostProcessingWindowInstance;
	VehicleDetailsWindow VehicleDetailsWindowInstance;
};
