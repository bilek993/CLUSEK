#pragma once
#include <string>
#include <windows.h>

class FileDialog final
{
public:
	static std::string OpenFile(const std::string& title, const std::string& filters);
	static std::string SaveFile(const std::string& title, const std::string& filters);
};
