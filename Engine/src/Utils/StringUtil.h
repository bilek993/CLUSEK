#pragma once
#include <string>

class StringUtil final
{
public:
	static std::wstring StringToWide(const std::string& input);
	static int StringToInt(const std::string& input);
};