#include "VehicleDetailsWindow.h"
#include <imgui.h>
#include "../../ECS/Components/VehicleComponent.h"

void VehicleDetailsWindow::Draw()
{
	ImGui::Begin("Vehicle details", &IsEnabled);

	DrawCombo();
	DrawVehicleDetails();
	DrawWheelsDetails();

	ImGui::End();
}

void VehicleDetailsWindow::DrawCombo()
{
	std::string vehicleIds;
	auto counter = 0;

	CurrentVehicleComponent = nullptr;

	Registry->view<VehicleComponent>().each([this, &vehicleIds, &counter](VehicleComponent& vehicleComponent)
	{
		vehicleIds += vehicleComponent.VehicleId + '\0';

		if (SelectedId == counter++)
		{
			CurrentVehicleComponent = &vehicleComponent;
		}
	});

	ImGui::Combo("Selected vehicle Id", &SelectedId, vehicleIds.c_str());
}

void VehicleDetailsWindow::DrawVehicleDetails()
{
	ImGui::Separator();

	if (CurrentVehicleComponent == nullptr)
	{
		ImGui::Text("No data!");
	}
	else
	{
		const auto& driveDynData = CurrentVehicleComponent->Vehicle->mDriveDynData;

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

		ImGui::Separator();

		const auto currentGear = driveDynData.getCurrentGear() == 1 ? 'N' : driveDynData.getCurrentGear() - 1 + '0';
		ImGui::Text("Current gear: %c", currentGear);

		const auto targetGear = driveDynData.getTargetGear() == 1 ? 'N' : driveDynData.getTargetGear() - 1 + '0';
		ImGui::Text("Target gear: %c", targetGear);

		ImGui::Text("Gearbox type: %s", driveDynData.getUseAutoGears() ? "automatic" : "manual");

		if (driveDynData.getUseAutoGears())
			ImGui::Text("Time since last automatic initialized gear change: %f", driveDynData.getAutoBoxSwitchTime());

		ImGui::Separator();

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
}

void VehicleDetailsWindow::DrawWheelsDetails() const
{
	if (CurrentVehicleComponent == nullptr)
		return;

	const auto wheelsCount = IM_ARRAYSIZE(CurrentVehicleComponent->Wheels);

	std::vector<float> wheelRotationSpeed(wheelsCount);
	std::vector<float> wheelRotationAngle(wheelsCount);

	for (auto i = 0; i < wheelsCount; i++)
	{
		wheelRotationSpeed[i] = CurrentVehicleComponent->Vehicle->mWheelsDynData.getWheelRotationSpeed(i);
		wheelRotationAngle[i] = CurrentVehicleComponent->Vehicle->mWheelsDynData.getWheelRotationAngle(i);
	}

	ImGui::Separator();

	ImGui::PlotHistogram(	"Wheels rotation speed", 
							wheelRotationSpeed.data(), 
							wheelsCount,
							0, 
							nullptr, 
							0, FLT_MAX, 
							ImVec2(0, 100));

	ImGui::PlotHistogram(	"Wheels rotation speed",
							wheelRotationAngle.data(),
							wheelsCount,
							0,
							nullptr,
							0, FLT_MAX,
							ImVec2(0, 100));
}

void VehicleDetailsWindow::RecalculateGraph(const float rotationSpeed)
{
	TimeCounter += DeltaTime;

	if (TimeCounter > 100.0f)
	{
		TimeCounter = 0.0f;
		AddToGraph(rotationSpeed);
	}
}

void VehicleDetailsWindow::AddToGraph(const float rotationSpeed)
{
	for (auto i = 0; i < IM_ARRAYSIZE(EngineRotationHistory) - 1; i++)
	{
		EngineRotationHistory[i] = EngineRotationHistory[i + 1];
	}

	EngineRotationHistory[IM_ARRAYSIZE(EngineRotationHistory) - 1] = rotationSpeed;
}
