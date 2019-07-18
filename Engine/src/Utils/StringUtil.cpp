#include "StringUtil.h"

std::wstring StringUtil::StringToWide(const std::string& input)
{
	std::wstring outputWideString(input.begin(), input.end());
	return outputWideString;
}

std::string StringUtil::FindExtension(std::string input)
{
	const auto position = input.find_last_of('.');

	if (position == std::string::npos)
		return "";

	std::transform(input.begin(), input.end(), input.begin(), ::toupper);
	return input.substr(position + 1);
}
