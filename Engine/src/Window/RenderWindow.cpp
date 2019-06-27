#include "RenderWindow.h"
#include "../Utils/Logger.h"

bool RenderWindow::Initialize(const HINSTANCE hInstance, const std::string& windowTitle, 
	const std::string& windowClass, const int width, const int height)
{
	HInstance = hInstance;
	WindowClass = windowClass;

	RegisterWindowClass();

	Logger::Debug("Creating window...");
	Handle = CreateWindowEx(
		0,
		WindowClass.c_str(),
		windowTitle.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		0,
		0,
		width,
		height,
		nullptr,
		nullptr,
		HInstance,
		nullptr);

	if (Handle == nullptr)
	{
		Logger::Error("Failed to create new window due to error: " + GetLastError());
		return false;
	}

	ShowWindow(Handle, SW_SHOW);
	SetForegroundWindow(Handle);
	SetFocus(Handle);

	Logger::Debug("Window initialization finished with success.");
	return true;
}

bool RenderWindow::ProcessMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	if (PeekMessage(&msg, Handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(Handle))
		{
			Logger::Debug("Closing window due to user request.");
			Handle = nullptr;
			UnregisterClass(WindowClass.c_str(), HInstance);
			return false;
		}
	}

	return true;
}

RenderWindow::~RenderWindow()
{
	if (Handle != nullptr)
	{
		UnregisterClass(WindowClass.c_str(), HInstance);
		DestroyWindow(Handle);
		Logger::Debug("Unregistered and destroyed window.");
	}
}

void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = HInstance;
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WindowClass.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	Logger::Debug("Registering window class...");
	RegisterClassEx(&wc);
}
