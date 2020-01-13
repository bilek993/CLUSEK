#pragma once
#include "BaseWindow.h"

class ConfigurationWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	std::string ConfigurationText{};
	bool RestartRequired = false;

	void DrawTextInput() const;
	void DrawSaveButton();
	void DrawInfoRestart() const;
};
