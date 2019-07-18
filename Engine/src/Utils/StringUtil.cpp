#include "StringUtil.h"

std::wstring StringUtil::StringToWide(const std::string& input)
{
	std::wstring outputWideString(input.begin(), input.end());
	return outputWideString;
}