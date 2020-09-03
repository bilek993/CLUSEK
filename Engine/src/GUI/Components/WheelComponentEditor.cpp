#include "WheelComponentEditor.h"
#include "../../Physics/VehicleResourcesGenerator.h"

void WheelComponentEditor::Draw()
{
	const auto wheelComponent = GetPointerToThisComponent();
	auto wheelsSimData = wheelComponent->Vehicle->mWheelsSimData;

	DrawBasicConfig(wheelComponent);
	ImGui::Separator();
	DrawWheelData(wheelsSimData, wheelComponent->WheelId);
	ImGui::Separator();
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
	wheelData.mMass = std::max(wheelData.mMass, EPSILON);
	ImGui::InputFloat("Radius", &wheelData.mRadius);
	wheelData.mRadius = std::max(wheelData.mRadius, EPSILON);
	ImGui::InputFloat("Width", &wheelData.mWidth);
	wheelData.mWidth = std::max(wheelData.mWidth, EPSILON);
	ImGui::InputFloat("Max brake torque", &wheelData.mMaxBrakeTorque);
	wheelData.mMaxBrakeTorque = std::max(wheelData.mMaxBrakeTorque, 0.0f);
	ImGui::InputFloat("Max handbrake torque", &wheelData.mMaxHandBrakeTorque);
	wheelData.mMaxHandBrakeTorque = std::max(wheelData.mMaxHandBrakeTorque, 0.0f);

	wheelData.mMOI = VehicleResourcesGenerator::CalculateWheelMOI(wheelData.mMass, wheelData.mRadius);

	wheelsSimData.setWheelData(wheelId, wheelData);
}

void WheelComponentEditor::DrawSuspensionData(physx::PxVehicleWheelsSimData& wheelsSimData, const int wheelId) const
{
	auto suspensionData = wheelsSimData.getSuspensionData(wheelId);

	ImGui::InputFloat("Max suspension compression", &suspensionData.mMaxCompression);
	suspensionData.mMaxCompression = std::max(suspensionData.mMaxCompression, 0.0f);
	ImGui::InputFloat("Max suspension droop", &suspensionData.mMaxDroop);
	suspensionData.mMaxDroop = std::max(suspensionData.mMaxDroop, EPSILON);
	ImGui::InputFloat("Spring strength", &suspensionData.mSpringStrength);
	suspensionData.mSpringStrength = std::max(suspensionData.mSpringStrength, EPSILON);
	ImGui::InputFloat("Spring damper rate", &suspensionData.mSpringDamperRate);
	suspensionData.mSpringDamperRate = std::max(suspensionData.mSpringDamperRate, 0.0f);

	ImGui::InputFloat("Camber at rest", &suspensionData.mCamberAtRest);
	ImGui::InputFloat("Camber at max droop", &suspensionData.mCamberAtMaxDroop);
	ImGui::InputFloat("Camber at max compression", &suspensionData.mCamberAtMaxCompression);

	wheelsSimData.setSuspensionData(wheelId, suspensionData);
}
