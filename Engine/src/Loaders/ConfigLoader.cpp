#include <fstream>
#include <algorithm>
#include "ConfigLoader.h"
#include "../Utils/Logger.h"
#include "../Utils/StringUtil.h"

ConfigData ConfigLoader::Data;
bool ConfigLoader::LoadedFromFile = false;
std::string ConfigLoader::FilePath = "Data/EngineSettings.cfg";

ConfigData& ConfigLoader::GetData()
{
	if (LoadedFromFile)
	{
		Logger::Debug("Configuration already exist. Returning already allocated data.");
		return Data;
	}

	Logger::Debug("Preparing to open file '" + FilePath + "'...");
	std::ifstream cfgFile(FilePath);

	if (cfgFile.is_open())
	{
		ConfigData newData;

		Logger::Debug("Reading configuration from file '" + FilePath + "'...");
		Logger::Debug(Logger::SEPARATOR);
		Logger::Debug("Loaded configuration:");

		std::string line;

		while (getline(cfgFile, line)) 
		{
			line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

			if (line[0] == '#' || line.empty())
				continue;

			const auto colonPosition = line.find(':');
			const auto key = line.substr(0, colonPosition);
			const auto value = line.substr(colonPosition + 1);

			if (key == "WindowWidth")
				newData.WindowWidth = StringUtil::StringToInt(value);
			else if (key == "WindowHeight")
				newData.WindowHeight = StringUtil::StringToInt(value);

			Logger::Debug("Value for '" + key + "' set to '" + value + "'.");
		}

		Logger::Debug(Logger::SEPARATOR);

		LoadedFromFile = true;
		Data = newData;
		Logger::Debug("Configuration has been successfully loaded!");
	}
	else
	{
		Logger::Warning("File '" + FilePath + "' doesn't exist!");
		Logger::Warning("Using default values for engine configuration!");
	}

	return Data;
}
