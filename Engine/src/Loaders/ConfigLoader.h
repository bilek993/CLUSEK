#pragma once
#include "ConfigData.h"

class ConfigLoader final
{
public:
	static ConfigData& GetData();
private:
	static ConfigData Data;
	static bool LoadedFromFile;
	static std::string FilePath;
};
