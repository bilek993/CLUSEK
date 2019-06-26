#include "Logger.h"
#include <Windows.h>
#include <chrono>

const std::string Logger::SEPARATOR = "-----------------------------------------------------------------------------------------------------";

FILE* Logger::Fp = nullptr;

void Logger::Initialize()
{
#ifdef _DEBUG
	AllocConsole();
	freopen_s(&Fp,"CONOUT$", "w", stdout);
#endif
}

void Logger::Free()
{
#ifdef _DEBUG
	fclose(Fp);
	FreeConsole();
#endif
}

void Logger::Debug(const std::string& input)
{
#ifdef _DEBUG
	printf(GenerateDebugString(input, "DBG").c_str());
#endif
}

void Logger::Warning(const std::string& input)
{
#ifdef _DEBUG
	printf(GenerateDebugString(input, "WRN").c_str());
#endif
}

void Logger::Error(const std::string& input)
{
#ifdef _DEBUG
	printf(GenerateDebugString(input, "ERR").c_str());
#endif
	MessageBox(nullptr, input.c_str(), "Fatal error", MB_ICONSTOP | MB_OK);
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