#pragma once
#include "BaseWindow.h"
#include <future>
#include "../../Utils/TextureBlockCompressionSettings.h"

class TextureCompressionWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawLoading() const;
	void DrawMain();

	void DrawInputSettingsTreeNode();
	void DrawMipMapsSettingsTreeNode();
	void DrawCompressionSettingsTreeNode();
	void DrawOutputSettingsTreeNode();
	void DrawMainActionButton();
	void DrawFileSelection();

	bool LoadingInProgress = false;

	std::string InputFilePath{};
	std::string OutputFilePath{};

	int SelectedAlgorithmId = 0;

	TextureBlockCompressionSettings CurrentCompressionSettings{};

	std::future<void> FutureCompression;
};
