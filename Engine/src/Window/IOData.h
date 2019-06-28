#pragma once
#include <Mouse.h>
#include <Keyboard.h>

struct IOData final
{
	DirectX::Mouse::State MouseState;
	DirectX::Keyboard::State KeyboardState;
	DirectX::Mouse::ButtonStateTracker MouseTracker;
	DirectX::Keyboard::KeyboardStateTracker KeyboardTracker;
};
