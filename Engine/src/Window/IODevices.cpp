#include "IODevices.h"
#include "../Utils/Logger.h"

IODevices::IODevices()
{
	Logger::Debug("Preparing IO Devices...");

	Mouse = std::make_unique<DirectX::Mouse>();
	Keyboard = std::make_unique<DirectX::Keyboard>();
	GamePad = std::make_unique<DirectX::GamePad>();
}

void IODevices::Update()
{
	MouseState = Mouse->GetState();
	KeyboardState = Keyboard->GetState();
	GamePadState = GamePad->GetState(0);

	MouseTracker.Update(MouseState);
	KeyboardTracker.Update(KeyboardState);
	if (GamePadState.IsConnected()) {
		GamePadTracker.Update(GamePadState);
	}

	if (CurrentMouseMode == DirectX::Mouse::Mode::MODE_ABSOLUTE)
	{
		MouseState.x = 0;
		MouseState.y = 0;
	}
}

void IODevices::ChangeMouseToRelativeMode(const HWND hwnd)
{
	if (CurrentMouseMode == DirectX::Mouse::MODE_RELATIVE)
		return;

	Mouse->SetWindow(hwnd);
	CurrentMouseMode = DirectX::Mouse::Mode::MODE_RELATIVE;
	Mouse->SetMode(CurrentMouseMode);
	Logger::Debug("Switched mouse into relative mode.");
}

void IODevices::ChangeMouseToAbsoluteMode(const HWND hwnd) // Absolute mode is default mode
{
	if (CurrentMouseMode == DirectX::Mouse::MODE_ABSOLUTE)
		return;

	Mouse->SetWindow(hwnd);
	CurrentMouseMode = DirectX::Mouse::Mode::MODE_ABSOLUTE;
	Mouse->SetMode(CurrentMouseMode);
	Logger::Debug("Switched mouse into absolute mode.");
}

IOData IODevices::Get() const
{
	return { MouseState, KeyboardState, GamePadState, MouseTracker, KeyboardTracker, GamePadTracker };
}

