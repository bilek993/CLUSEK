#pragma once
#include <Windows.h> // Required as first include due to #if (!defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)) && defined(WM_USER) in Mouse.h
#include <Mouse.h>
#include <Keyboard.h>
#include <GamePad.h>
#include "IOData.h"

class IODevices final
{
public:
	IODevices();
	void Update();
	void ChangeMouseToRelativeMode(HWND hwnd);
	void ChangeMouseToAbsoluteMode(HWND hwnd);
	IOData Get() const;

private:
	DirectX::Mouse::Mode CurrentMouseMode;

	std::unique_ptr<DirectX::Mouse> Mouse;
	std::unique_ptr<DirectX::Keyboard> Keyboard;
	std::unique_ptr<DirectX::GamePad> GamePad;

	DirectX::Mouse::State MouseState;
	DirectX::Keyboard::State KeyboardState;
	DirectX::GamePad::State GamePadState;

	DirectX::Mouse::ButtonStateTracker MouseTracker;
	DirectX::Keyboard::KeyboardStateTracker KeyboardTracker;
	DirectX::GamePad::ButtonStateTracker GamePadTracker;
};
