#include "StringUtil.h"

std::wstring StringUtil::StringToWide(const std::string& input)
{
	std::wstring outputWideString(input.begin(), input.end());
	return outputWideString;
}


int StringUtil::StringToInt(const std::string& input)
{
	return std::stoi(input);
}
