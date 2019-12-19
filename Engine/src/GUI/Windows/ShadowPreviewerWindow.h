#pragma once
#include "BaseWindow.h"

class ShadowPreviewerWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawPreview() const;
	void DrawSettings();

	float ImageScale = 0.5f;
};
