#pragma once
#include "ConfigData.h"

class ConfigLoader final
{
public:
	static ConfigData GetData();
private:
	static std::string FilePath;
};
