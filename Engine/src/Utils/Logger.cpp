#include "Logger.h"
#include <Windows.h>
#include <chrono>

FILE* Logger::Fp = nullptr;

void Logger::Initialize()
{
	AllocConsole();
	freopen_s(&Fp,"CONOUT$", "w", stdout);
}

void Logger::Free()
{
	fclose(Fp);
	FreeConsole();
}

void Logger::Debug(const std::string& input)
{
	printf(GenerateDebugString(input, "DBG").c_str());
}

void Logger::Warning(const std::string& input)
{
	printf(GenerateDebugString(input, "WRN").c_str());
}

void Logger::Error(const std::string& input)
{
	printf(GenerateDebugString(input, "ERR").c_str());
	MessageBox(nullptr, input.c_str(), "Critical error!", MB_ICONSTOP | MB_OK);
}

std::string Logger::GenerateDebugString(const std::string& input, const std::string& level)
{
	auto now = time(nullptr);
	char timeBuffer[10];
	std::tm bt{};
	localtime_s(&bt, &now);
	strftime(timeBuffer, 10, "%H:%M:%S", &bt);

	return std::string(timeBuffer) + " | " + level + " | " + input + "\n";
}