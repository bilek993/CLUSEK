#pragma once
#include "Window/RenderWindow.h"
#include "Loaders/ConfigData.h"
#include "Window/IODevices.h"
#include "Renderer/Renderer.h"

class Engine final
{
public:
	bool Initialize(HINSTANCE hInstance, const ConfigData configData);
	bool CanUpdate();
	void Update();
	void Render();
private:
	void UpdateInputOutputDevices();
	void HandleClosingWithButton();

	RenderWindow Window;
	ConfigData Config;

	Renderer GraphicsRenderer;

	IODevices InputOutputDevices;
	IOData DataFromIODevices = IOData();
};
