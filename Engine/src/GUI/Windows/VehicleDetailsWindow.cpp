#include "VehicleDetailsWindow.h"
#include <imgui.h>
#include "../../ECS/Components/VehicleComponent.h"

void VehicleDetailsWindow::Draw()
{
	ImGui::Begin("Vehicle details", &IsEnabled);

	DrawCombo();
	DrawDetails();

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

void VehicleDetailsWindow::DrawDetails() const
{
	ImGui::Separator();

	if (CurrentVehicleComponent == nullptr)
	{
		ImGui::Text("No data!");
	}
	else
	{
		ImGui::Text("Acceleration");
		ImGui::SameLine();
		ImGui::ProgressBar(CurrentVehicleComponent->Vehicle->mDriveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL));
		
		ImGui::Text("Braking");
		ImGui::SameLine();
		ImGui::ProgressBar(CurrentVehicleComponent->Vehicle->mDriveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE));

		ImGui::Text("Handbrake");
		ImGui::SameLine();
		ImGui::ProgressBar(CurrentVehicleComponent->Vehicle->mDriveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_HANDBRAKE));

		ImGui::Text("Steering");
		ImGui::SameLine();
		ImGui::ProgressBar((-CurrentVehicleComponent->Vehicle->mDriveDynData.getAnalogInput(physx::PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT) + 1) / 2);

		ImGui::Separator();
	}
}
