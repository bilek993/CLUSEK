#pragma once

struct ConfigData final
{
	// Logger settings
	int LoggerEnabled = 0;
	std::string LoggerDestination = "logs.txt";

	// Window settings
	std::string WindowClassName = "EngineClassName";
	std::string WindowTitle = "";
	int WindowWidth = 640;
	int WindowHeight = 480;

	// DirectX settings
	int Fullscreen = 0;
	int SyncIntervals = 1;
	int SelectedAdapterId = 0;
	int RefreshRateNumerator = 0;
	int RefreshRateDenominator = 0;
	int MultisamplesCount = 1;
	int MultisamplesQuality = 0;
};