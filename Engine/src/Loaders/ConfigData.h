#pragma once

struct ConfigData final
{
	// Window settings
	std::string WindowClassName = "EngineClassName";
	std::string WindowTitle = "";
	int WindowWidth = 640;
	int WindowHeight = 480;

	// DirectX settings
	int SelectedAdapterId = 0;
};