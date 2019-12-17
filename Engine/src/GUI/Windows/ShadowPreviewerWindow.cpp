#include "ShadowPreviewerWindow.h"
#include <imgui.h>
#include "../../ECS/Systems/RenderSystem.h"

void ShadowPreviewerWindow::Draw()
{
	ImGui::Begin("Shadow previewer", &IsEnabled);

	const auto renderSystem = dynamic_cast<RenderSystem*>((*Systems)[RenderSystemId].System.get());
	ImGui::Image(static_cast<void*>(renderSystem->GetPointerToShadowShaderResourceView()), ImVec2(Config->ShadowsWidth, Config->ShadowsHeight));

	ImGui::End();
}
