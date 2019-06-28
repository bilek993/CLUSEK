#pragma once
#include <Windows.h> // Required as first include due to #if (!defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)) && defined(WM_USER) in Mouse.h
#include <Mouse.h>
#include <Keyboard.h>
#include "IOData.h"

class IODevices final
{
public:
	IODevices();
	void Update();
	void ChangeMouseToRelativeMode(HWND hwnd) const;
	void ChangeMouseToAbsoluteMode(HWND hwnd) const;
	IOData Get() const;

private:
	std::unique_ptr<DirectX::Mouse> Mouse;
	std::unique_ptr<DirectX::Keyboard> Keyboard;

	DirectX::Mouse::State MouseState;
	DirectX::Keyboard::State KeyboardState;

	DirectX::Mouse::ButtonStateTracker MouseTracker;
	DirectX::Keyboard::KeyboardStateTracker KeyboardTracker;
};
