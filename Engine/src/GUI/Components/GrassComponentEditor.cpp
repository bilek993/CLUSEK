#include "GrassComponentEditor.h"

void GrassComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::SliderFloat("Alpha discard point", &component->AlphaDiscardPoint, 0.0f, 1.0f);

	ImGui::Separator();

	ImGui::SliderFloat("Albedo interpolation ratio", &component->AlbedoInterpolationRatio, 0.0f, 1.0f);

	ImGui::Separator();
	
	ImGui::SliderFloat("Normal interpolation ratio", &component->NormalInterpolationRatio, 0.0f, 1.0f);

	ImGui::Separator();

	ImGui::SliderFloat("Roughness interpolation target", &component->RoughnessInterpolationTarget, 0.0f, 1.0f);
	ImGui::SliderFloat("Roughness interpolation ratio", &component->RoughnessInterpolationRatio, 0.0f, 1.0f);

	ImGui::Separator();

	ImGui::SliderFloat("Metalic interpolation target", &component->MetalicInterpolationTarget, 0.0f, 1.0f);
	ImGui::SliderFloat("Metalic interpolation ratio", &component->MetalicInterpolationRatio, 0.0f, 1.0f);

	ImGui::Separator();

	ImGui::SliderFloat("Occlusion value", &component->OcclusionValue, 0.0f, 1.0f);
}
