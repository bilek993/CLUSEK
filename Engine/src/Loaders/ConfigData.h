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
	int WireframeMode = 0;
	std::string CullMode = "BACK";

	// Main camera settings
	float MainCameraFov = 90.0f;
	float MaxRotationX = 90.0f;
	float MinRotationX = -90.0f;
	float MainCameraNearZ = 0.1f;
	float MainCameraFarZ = 1000.0f;

	// Files responsible for world description
	std::string PathToResources = "";
	std::string PathToMap = "";
};