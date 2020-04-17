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
	void DrawCompressionSettingsTreeNode() const;
	void DrawOutputSettingsTreeNode() const;
	void DrawMainActionButton();
	void DrawFileSelection();

	bool LoadingInProgress = false;

	std::string InputFilePath{};
	std::string OutputFilePath{};

	TextureBlockCompressionSettings CurrentCompressionSettings{};

	std::future<void> FutureCompression;
};
