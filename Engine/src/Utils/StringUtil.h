#pragma once
#include <string>
#include <algorithm>

class StringUtil final
{
public:
	static std::wstring StringToWide(const std::string& input);
	static std::string FindExtension(std::string input);
};