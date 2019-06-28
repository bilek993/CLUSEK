#include "IODevices.h"
#include "../Utils/Logger.h"

IODevices::IODevices()
{
	Logger::Debug("Preparing IO Devices...");

	Mouse = std::make_unique<DirectX::Mouse>();
	Keyboard = std::make_unique<DirectX::Keyboard>();
}

void IODevices::Update()
{
	MouseState = Mouse->GetState();
	KeyboardState = Keyboard->GetState();

	MouseTracker.Update(MouseState);
	KeyboardTracker.Update(KeyboardState);
}

IOData IODevices::Get() const
{
	return { MouseState, KeyboardState, MouseTracker, KeyboardTracker };
}

