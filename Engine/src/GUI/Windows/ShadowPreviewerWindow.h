#pragma once
#include "BaseWindow.h"
#include "../../ECS/Systems/RenderSystem.h"

class ShadowPreviewerWindow final : public BaseWindow
{
protected:
	void Draw() override;

private:
	void DrawSettings();
	void DrawPreview() const;
	void DrawWarning() const;

	int CascadeLevel = ShadowCamera::SHADOW_CASCADES_COUNT - 1;
	float ImageScale = 0.5f;
	float Bias = 2.0f;
};
