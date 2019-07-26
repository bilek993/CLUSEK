#pragma once
#include "ConfigData.h"

#define MAP_CONFIG_FIELD(INPUT_KEY, EXPECTED_KEY, CONFIG_DATA_OBJECT_NAME, CONVERTED_VALUE) if (INPUT_KEY == #EXPECTED_KEY) CONFIG_DATA_OBJECT_NAME.EXPECTED_KEY = CONVERTED_VALUE;

class ConfigLoader final
{
public:
	static ConfigData GetData();
private:
	inline static std::string FilePath = "Data/EngineSettings.cfg";
};
