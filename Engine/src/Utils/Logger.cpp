#include "Logger.h"
#include <Windows.h>
#include <chrono>

void Logger::Initialize(const bool enabled, const std::string& destination)
{
	if (!enabled)
		return;

	if (IsEnabled)
	{
		Error("Logger already initialized! Cannot allocate two loggers at once.");
		return;
	}

	if (destination == "CONOUT$")
		AllocConsole();

	IsEnabled = enabled;
	Destination = destination;

	freopen_s(&Fp, Destination.c_str(), "w", stdout);
}

void Logger::Free()
{
	if (!IsEnabled)
		return;

	fclose(Fp);
	FreeConsole();
}

void Logger::Debug(const std::string& input)
{
	if (!IsEnabled)
		return;

	printf(GenerateDebugString(input, "DBG").c_str());
}

void Logger::Warning(const std::string& input)
{
	if (!IsEnabled)
		return;

	printf(GenerateDebugString(input, "WRN").c_str());
}

void Logger::Error(const std::string& input)
{
	MessageBox(nullptr, input.c_str(), "Fatal error", MB_ICONSTOP | MB_OK);

	if (IsEnabled)
		printf(GenerateDebugString(input, "ERR").c_str());
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