#pragma once
#include <string>

class Logger final
{
public:
	static void Initialize(bool enabled, const std::string& destination);
	static void Free();

	static void SetLevel(int level);

	static void Debug(const std::string& input);
	static void Warning(const std::string& input);
	static void Error(const std::string& input);
private:
	inline static const std::string CONSOLE_OUTPUT = "CONOUT$";
	inline static const std::string FILE_MODE = "w";

	inline static FILE* Fp = nullptr;
	inline static bool IsEnabled = false;
	inline static std::string Destination;
	inline static int Level = 0;

	static std::string GenerateDebugString(const std::string& input, const std::string& level);
};
