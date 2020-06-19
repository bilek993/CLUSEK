#include "WheelComponentEditor.h"
#include "../../Physics/VehicleResourcesGenerator.h"

void WheelComponentEditor::Draw()
{
	const auto wheelComponent = GetPointerToThisComponent();
	auto wheelsSimData = wheelComponent->Vehicle->mWheelsSimData;

	DrawBasicConfig(wheelComponent);
	DrawWheelData(wheelsSimData, wheelComponent->WheelId);
	DrawSuspensionData(wheelsSimData, wheelComponent->WheelId);
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

void WheelComponentEditor::DrawSuspensionData(physx::PxVehicleWheelsSimData& wheelsSimData, const int wheelId) const
{
	auto suspensionData = wheelsSimData.getSuspensionData(wheelId);

	ImGui::InputFloat("Max suspension compression", &suspensionData.mMaxCompression);
	ImGui::InputFloat("Max suspension droop", &suspensionData.mMaxDroop);
	ImGui::InputFloat("Spring strength", &suspensionData.mSpringStrength);
	ImGui::InputFloat("Spring damper rate", &suspensionData.mSpringDamperRate);

	ImGui::InputFloat("Camber at rest", &suspensionData.mCamberAtRest);
	ImGui::InputFloat("Camber at max droop", &suspensionData.mCamberAtMaxDroop);
	ImGui::InputFloat("Camber at max compression", &suspensionData.mCamberAtMaxCompression);

	wheelsSimData.setSuspensionData(wheelId, suspensionData);
}
