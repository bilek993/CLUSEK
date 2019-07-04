#pragma once
#include <string>

class Logger final
{
public:
	static void Initialize(bool enabled, const std::string& destination);
	static void Free();

	static void Debug(const std::string& input);
	static void Warning(const std::string& input);
	static void Error(const std::string& input);
private:
	static FILE* Fp;
	static bool IsEnabled;
	static std::string Destination;

	static std::string GenerateDebugString(const std::string& input, const std::string& level);
};
