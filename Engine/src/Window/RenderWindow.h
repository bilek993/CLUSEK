#pragma once
#include <Windows.h>
#include <string>

class RenderWindow final
{
public:
	bool Initialize(HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height);
	bool ProcessMessages();
	~RenderWindow();
private:
	void RegisterWindowClass();

	HWND Handle = nullptr;
	HINSTANCE HInstance = nullptr;
	std::string WindowTitle = "";
	std::string WindowClass = "";
};
