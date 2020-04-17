#pragma once
#include "BaseWindow.h"

class TextureCompressionWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawLoading() const;
	void DrawMain();

	void DrawMipMapsSettingsTreeNode() const;
	void DrawCompressionSettingsTreeNode() const;
	void DrawOutputSettingsTreeNode() const;
	void DrawMainActionButton() const;
	void DrawFileSelection();

	bool LoadingInProgress = false;

	std::string InputFilePath{};
	std::string OutputFilePath{};
};
