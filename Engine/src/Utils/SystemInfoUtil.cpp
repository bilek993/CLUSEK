#include <windows.h>
#include "SystemInfoUtil.h"
#include "Logger.h"

void SystemInfoUtil::LogInfo()
{
	Logger::Debug("Getting system info...");
	SYSTEM_INFO info;
	GetSystemInfo(&info);

	Logger::Debug("Number of processors: " + std::to_string(info.dwNumberOfProcessors));
	Logger::Debug("Page size: " + std::to_string(info.dwPageSize));
	Logger::Debug("Processor architecture: " + std::to_string(info.wProcessorArchitecture));
	Logger::Debug("Processor type: " + std::to_string(info.dwProcessorType));
	Logger::Debug("Processor level: " + std::to_string(info.wProcessorLevel));
	Logger::Debug("Processor revision: " + std::to_string(info.wProcessorRevision));
	Logger::Debug("Active processor mask: " + std::to_string(info.dwActiveProcessorMask));
	Logger::Debug("Allocation Granularity: " + std::to_string(info.dwAllocationGranularity));
}
