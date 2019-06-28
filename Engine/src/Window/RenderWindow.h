#pragma once
#include <Windows.h>
#include <string>

class RenderWindow final
{
public:
	bool Initialize(HINSTANCE hInstance, const std::string& windowTitle, const std::string& windowClass, int width, int height);
	bool ProcessMessages();
	HWND GetHandle() const;
	~RenderWindow();
private:
	void RegisterWindowClass() const;

	HWND Handle = nullptr;
	HINSTANCE HInstance = nullptr;
	std::string WindowClass = "";
};
