#include "VehicleComponentEditor.h"

void VehicleComponentEditor::Draw()
{
	const auto component = GetPointerToThisComponent();

	ImGui::Text("Statistics:");
	ImGui::Spacing();
	DrawVehicleStats(component);
	DrawWheelsStats(component);

	ImGui::Separator();

	ImGui::Text("Editable parameters:");
	ImGui::Spacing();
	DrawEditableParameters(component);
}

void VehicleComponentEditor::DrawVehicleStats(VehicleComponent* component)
{
	const auto& driveDynData = component->Vehicle->mDriveDynData;

	ImGui::Text("Acceleration");
	ImGui::SameLine();
	ImGui::ProgressBar(driveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL));

	ImGui::Text("Braking");
	ImGui::SameLine();
	ImGui::ProgressBar(driveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE));

	ImGui::Text("Handbrake");
	ImGui::SameLine();
	ImGui::ProgressBar(driveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_HANDBRAKE));

	ImGui::Text("Steering");
	ImGui::SameLine();
	ImGui::ProgressBar((-driveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT) + 1) / 2);

	const auto currentGear = driveDynData.getCurrentGear() == 1 ? 'N' : driveDynData.getCurrentGear() - 1 + '0';
	ImGui::Text("Current gear: %c", currentGear);

	const auto targetGear = driveDynData.getTargetGear() == 1 ? 'N' : driveDynData.getTargetGear() - 1 + '0';
	ImGui::Text("Target gear: %c", targetGear);

	ImGui::Text("Gearbox type: %s", driveDynData.getUseAutoGears() ? "automatic" : "manual");

	if (driveDynData.getUseAutoGears())
		ImGui::Text("Time since last automatic initialized gear change: %f", driveDynData.getAutoBoxSwitchTime());

	ImGui::Text("Engine rotation speed: %f", driveDynData.getEngineRotationSpeed());
	RecalculateGraph(driveDynData.getEngineRotationSpeed());
	ImGui::PlotLines(	"Engine rotation speed history",
						EngineRotationHistory,
						IM_ARRAYSIZE(EngineRotationHistory),
						0, nullptr,
						0.0f,
						FLT_MAX,
						ImVec2(0, 100));
}

void VehicleComponentEditor::DrawWheelsStats(VehicleComponent* component) const
{
	const auto wheelsCount = IM_ARRAYSIZE(component->Wheels);

	std::vector<float> wheelRotationSpeed(wheelsCount);
	std::vector<float> wheelRotationAngle(wheelsCount);

	for (auto i = 0; i < wheelsCount; i++)
	{
		wheelRotationSpeed[i] = component->Vehicle->mWheelsDynData.getWheelRotationSpeed(i);
		wheelRotationAngle[i] = component->Vehicle->mWheelsDynData.getWheelRotationAngle(i);
	}

	ImGui::PlotHistogram(	"Wheels rotation speed",
							wheelRotationSpeed.data(),
							wheelsCount,
							0,
							nullptr,
							0, FLT_MAX,
							ImVec2(0, 100));

	ImGui::PlotHistogram(	"Wheels rotation angle",
							wheelRotationAngle.data(),
							wheelsCount,
							0,
							nullptr,
							0, FLT_MAX,
							ImVec2(0, 100));
}

void VehicleComponentEditor::DrawEditableParameters(VehicleComponent* component) const
{
	auto automaticGearbox = component->Vehicle->mDriveDynData.getUseAutoGears();
	ImGui::Checkbox("Use automatic gearbox", &automaticGearbox);
	component->Vehicle->mDriveDynData.setUseAutoGears(automaticGearbox);
}

void VehicleComponentEditor::RecalculateGraph(const float rotationSpeed)
{
	TimeCounter += DeltaTime;

	if (TimeCounter > 100.0f)
	{
		TimeCounter = 0.0f;
		AddToGraph(rotationSpeed);
	}
}

void VehicleComponentEditor::AddToGraph(const float rotationSpeed)
{
	for (auto i = 0; i < IM_ARRAYSIZE(EngineRotationHistory) - 1; i++)
	{
		EngineRotationHistory[i] = EngineRotationHistory[i + 1];
	}

	EngineRotationHistory[IM_ARRAYSIZE(EngineRotationHistory) - 1] = rotationSpeed;
}
