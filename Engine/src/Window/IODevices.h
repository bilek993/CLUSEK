#pragma once
#include <Mouse.h>
#include <Keyboard.h>
#include "IOData.h"

class IODevices final
{
public:
	IODevices();
	void Update();
	IOData Get() const;

private:
	std::unique_ptr<DirectX::Mouse> Mouse;
	std::unique_ptr<DirectX::Keyboard> Keyboard;

	DirectX::Mouse::State MouseState;
	DirectX::Keyboard::State KeyboardState;

	DirectX::Mouse::ButtonStateTracker MouseTracker;
	DirectX::Keyboard::KeyboardStateTracker KeyboardTracker;
};
