#include <fstream>
#include <algorithm>
#include "ConfigLoader.h"
#include "../Utils/Logger.h"
#include "../Utils/StringUtil.h"

std::string ConfigLoader::FilePath = "Data/EngineSettings.cfg";

ConfigData ConfigLoader::GetData()
{
	Logger::Debug("Preparing to open file '" + FilePath + "'...");
	std::ifstream cfgFile(FilePath);
	ConfigData data;

	if (cfgFile.is_open())
	{
		Logger::Debug("Reading configuration from file '" + FilePath + "'...");

		std::string line;

		while (getline(cfgFile, line)) 
		{
			line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

			if (line[0] == '#' || line.empty())
				continue;

			const auto colonPosition = line.find(':');
			auto key = line.substr(0, colonPosition);
			auto value = line.substr(colonPosition + 1);

			if (key == "WindowClassName")
				data.WindowClassName = value;
			else if (key == "WindowTitle")
				data.WindowTitle = value;
			else if (key == "WindowWidth")
				data.WindowWidth = std::stoi(value);
			else if (key == "WindowHeight")
				data.WindowHeight = std::stoi(value);
			else if (key == "SelectedAdapterId")
				data.SelectedAdapterId = std::stoi(value);
			else
				continue;

			Logger::Debug("New value '" + value + "' for key '" + key + "' has been set from configuration file.");
		}

		Logger::Debug("Configuration has been successfully loaded!");
	}
	else
	{
		Logger::Warning("File '" + FilePath + "' doesn't exist!");
		Logger::Warning("Using default values for engine configuration!");
	}

	return data;
}
