#pragma once
#include <Windows.h>
#include <string>

class RenderWindow final
{
public:
	bool Initialize(HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height);
	bool ProcessMessages();
	HWND GetHandle() const;
	void UserRequestedQuit();
	~RenderWindow();
private:
	void RegisterWindowClass() const;
	void Destroy();

	HWND Handle = nullptr;
	HINSTANCE HInstance = nullptr;
	std::string WindowClass = "";
};
