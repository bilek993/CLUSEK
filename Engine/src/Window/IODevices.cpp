#include "IODevices.h"
#include "../Utils/Logger.h"
#include <imgui.h>

IODevices::IODevices()
{
	Logger::Debug("Preparing IO Devices...");

	Mouse = std::make_unique<DirectX::Mouse>();
	Keyboard = std::make_unique<DirectX::Keyboard>();
	GamePad = std::make_unique<DirectX::GamePad>();
}

void IODevices::Update()
{
	auto& io = ImGui::GetIO();

	if (!io.WantCaptureMouse)
		HandleMouseUpdate();

	if (!io.WantCaptureKeyboard)
		HandleKeyboardUpdate();

	HandleGamePadUpdate();
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

void IODevices::HandleMouseUpdate()
{
	MouseState = Mouse->GetState();
	MouseTracker.Update(MouseState);

	if (CurrentMouseMode == DirectX::Mouse::Mode::MODE_ABSOLUTE)
	{
		MouseState.x = 0;
		MouseState.y = 0;
	}
}

void IODevices::HandleKeyboardUpdate()
{
	KeyboardState = Keyboard->GetState();
	KeyboardTracker.Update(KeyboardState);
}

void IODevices::HandleGamePadUpdate()
{
	GamePadState = GamePad->GetState(0);
	if (GamePadState.IsConnected()) {
		GamePadTracker.Update(GamePadState);
	}
}

IOData IODevices::Get() const
{
	return { MouseState, KeyboardState, GamePadState, MouseTracker, KeyboardTracker, GamePadTracker };
}

