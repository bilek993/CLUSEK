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

void IODevices::ChangeMouseToRelativeMode(const HWND hwnd) const
{
	Mouse->SetWindow(hwnd);
	Mouse->SetMode(DirectX::Mouse::Mode::MODE_RELATIVE);
	Logger::Debug("Switched mouse into relative mode.");
}

void IODevices::ChangeMouseToAbsoluteMode(const HWND hwnd) const
{
	Mouse->SetWindow(hwnd);
	Mouse->SetMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);
	Logger::Debug("Switched mouse into absolute mode.");
}

IOData IODevices::Get() const
{
	return { MouseState, KeyboardState, MouseTracker, KeyboardTracker };
}

