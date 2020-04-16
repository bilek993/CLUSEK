#include "TextureCompressionWindow.h"
#include <imgui.h>

void TextureCompressionWindow::Draw()
{
	ImGui::Begin("Texture compression", &IsEnabled);

	if (!LoadingInProgress)
		DrawMain();
	else
		DrawLoading();

	ImGui::End();
}

void TextureCompressionWindow::DrawLoading() const
{
	ImGui::SetCursorPosX((ImGui::GetWindowSize().x / 2) - 31);
	ImGui::SetCursorPosY((ImGui::GetWindowSize().y / 2));
	ImGui::Text("LOADING %c", "|/-\\"[static_cast<int>(ImGui::GetTime() / 0.15f) & 3]);
}

void TextureCompressionWindow::DrawMain() const
{
	ImGui::Text("TODO: Add content!");
}
