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
	void Initialize(const HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext) const;
	void Update(const IOData& ioData, std::vector<SystemHolder>& systems);

	~DebugUserInterface();
private:
	void HandleKeyboardEvents(const IOData& ioData);

	void BeforeUpdate() const;
	void AfterUpdate() const;

	void DrawMenuBar() const;
	void DrawSystemBrowser(std::vector<SystemHolder>& systems) const;

	bool IsEnabled = false;
};
