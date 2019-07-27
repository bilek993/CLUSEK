#pragma once
#include <vector>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "../Utils/SystemHolder.h"
#include <d3d11.h>

class DebugUserInterface final
{
public:
	void Initialize(const HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		const std::function<void()> &functionCloseEngine);
	void Update(float deltaTime, const IOData& ioData, std::vector<SystemHolder>& systems, 
		DynamicRenderSettings& dynamicRenderSettings);

	~DebugUserInterface();
private:
	void HandleKeyboardEvents(const IOData& ioData);
	void HandleMainDockingArea();

	void BeforeUpdate() const;
	void AfterUpdate() const;

	void DrawMenuBar() const;

	bool IsEnabled = false;
	bool IsDockingEnabled = false;

	std::function<void()> FunctionCloseEngine;
};
