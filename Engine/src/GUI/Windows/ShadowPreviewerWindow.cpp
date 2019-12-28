#include "ShadowPreviewerWindow.h"
#include <imgui.h>
#include "../../ECS/Systems/RenderSystem.h"

void ShadowPreviewerWindow::Draw()
{
	ImGui::Begin("Shadow previewer", &IsEnabled);

	DrawSettings();
	ImGui::Separator();
	DrawPreview();

	ImGui::End();
}

void ShadowPreviewerWindow::DrawSettings()
{
	ImGui::Text("Settings:");
	ImGui::SliderFloat("Image scale", &ImageScale, 0.1f, 10.0f);
	ImGui::SliderFloat("Bias", &Bias, 0.1f, 10000.0f);
}

void ShadowPreviewerWindow::DrawPreview() const
{
	const auto renderSystem = dynamic_cast<RenderSystem*>((*Systems)[RenderSystemId].System.get());

	ImGui::Text("Preview:");
	ImGui::Image(static_cast<void*>(renderSystem->GetPointerToRemappedShadowShaderResourceView(Bias)), 
		ImVec2(Config->ShadowsWidth * ImageScale, Config->ShadowsHeight * ImageScale));
}
