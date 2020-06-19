#include "WheelComponentEditor.h"
#include "../../Physics/VehicleResourcesGenerator.h"

void WheelComponentEditor::Draw()
{
	const auto wheelComponent = GetPointerToThisComponent();
	auto wheelsSimData = wheelComponent->Vehicle->mWheelsSimData;

	DrawBasicConfig(wheelComponent);
	DrawWheelData(wheelsSimData, wheelComponent->WheelId);
}

void WheelComponentEditor::DrawBasicConfig(WheelComponent* wheelComponent) const
{
	ImGui::Checkbox("Flip", &wheelComponent->Flip);
}

void WheelComponentEditor::DrawWheelData(physx::PxVehicleWheelsSimData& wheelsSimData, const int wheelId) const
{
	auto wheelData = wheelsSimData.getWheelData(wheelId);

	ImGui::InputFloat("Mass", &wheelData.mMass);
	ImGui::InputFloat("Radius", &wheelData.mRadius);
	ImGui::InputFloat("Width", &wheelData.mWidth);
	ImGui::InputFloat("Max brake torque", &wheelData.mMaxBrakeTorque);
	ImGui::InputFloat("Max handbrake torque", &wheelData.mMaxHandBrakeTorque);

	wheelData.mMOI = VehicleResourcesGenerator::CalculateWheelMOI(wheelData.mMass, wheelData.mRadius);

	wheelsSimData.setWheelData(wheelId, wheelData);
}
