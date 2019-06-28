#pragma once
#include "Window/RenderWindow.h"
#include "Loaders/ConfigData.h"
#include "Window/IODevices.h"
#include "Renderer/Renderer.h"

class Engine final
{
public:
	bool Initialize(HINSTANCE hInstance);
	bool CanUpdate();
	void Update();
private:
	void UpdateInputOutputDevices();
	void HandleBasicInputOutput();

	RenderWindow Window;
	ConfigData Config;

	Renderer GraphicsRenderer;

	IODevices InputOutputDevices;
	IOData DataFromIODevices = IOData();
};
