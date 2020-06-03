#include "RenderWindow.h"
#include "../Utils/Logger.h"
#include <Keyboard.h>
#include <Mouse.h>
#include <winuser.h>
#include "../resource.h"

bool RenderWindow::Initialize(const HINSTANCE hInstance, const std::string& windowTitle, 
	const std::string& windowClass, const int width, const int height)
{
	HInstance = hInstance;
	WindowClass = windowClass;

	RegisterWindowClass();

	Logger::Debug("Creating window...");

	RECT windowRect;
	windowRect.left = (GetSystemMetrics(SM_CXSCREEN) / 2) - (width / 2);
	windowRect.top = (GetSystemMetrics(SM_CYSCREEN) / 2) - (height / 2);
	windowRect.right = windowRect.left + width;
	windowRect.bottom = windowRect.top + height;

	const auto windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	AdjustWindowRect(&windowRect, windowStyle, false);

	Handle = CreateWindowEx(0,
							WindowClass.c_str(),
							windowTitle.c_str(),
							windowStyle,
							windowRect.left,
							windowRect.top,
							windowRect.right - windowRect.left,
							windowRect.bottom - windowRect.top,
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

	while (PeekMessage(&msg, Handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(Handle))
		{
			Destroy();
			return false;
		}
	} 
	else if (msg.message == WM_CLOSE)
	{
		Destroy();
		return false;
	}

	return true;
}

HWND RenderWindow::GetHandle() const
{
	return Handle;
}

void RenderWindow::UserRequestedQuit() const
{
	SendMessage(Handle, WM_CLOSE, 0, 0);
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

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(const HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_ACTIVATEAPP:
		DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void RenderWindow::RegisterWindowClass() const
{
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = HInstance;
	wc.hIcon = static_cast<HICON>(LoadImage(HInstance, MAKEINTRESOURCE(APP_ICON), IMAGE_ICON, 256, 256, 0));
	wc.hIconSm = static_cast<HICON>(LoadImage(HInstance, MAKEINTRESOURCE(APP_ICON), IMAGE_ICON, 16, 16, 0));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WindowClass.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	Logger::Debug("Registering window class...");
	RegisterClassEx(&wc);
}

void RenderWindow::Destroy()
{
	Logger::Debug("Closing window due to user request.");
	Handle = nullptr;
	UnregisterClass(WindowClass.c_str(), HInstance);
}
