#pragma once
#include "ConfigData.h"

class ConfigLoader final
{
public:
	static ConfigData GetData();
private:
	inline static std::string FilePath = "Data/EngineSettings.cfg";
};
