#pragma once
#include "BaseWindow.h"

class ConfigurationWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	inline static std::string ConfigFilePath = "Data/EngineSettings.cfg";

	std::string ConfigurationText{};
	bool RestartRequired = false;
	bool ConfigLoaded = false;

	void LoadConfiguration();
	void SaveConfiguration() const;

	void DrawTextInput();
	void DrawSaveButton();
	void DrawInfoRestart() const;
};
