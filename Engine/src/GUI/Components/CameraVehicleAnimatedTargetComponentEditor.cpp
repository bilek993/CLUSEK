#include "CameraVehicleAnimatedTargetComponentEditor.h"

void CameraVehicleAnimatedTargetComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::InputFloat("Power", &component->Power);
	ImGui::InputFloat("Smoothness", &component->Smoothness);

	DrawAnimationPositionVectorEditor(component);
}

void CameraVehicleAnimatedTargetComponentEditor::DrawAnimationPositionVectorEditor(CameraVehicleAnimatedTargetComponent* component) const
{
	DirectX::XMFLOAT3 animationPositionFloats{};
	XMStoreFloat3(&animationPositionFloats, component->AnimationPositionVector);

	ImGui::InputFloat3("Animation position", &animationPositionFloats.x);

	component->AnimationPositionVector = XMLoadFloat3(&animationPositionFloats);
}
