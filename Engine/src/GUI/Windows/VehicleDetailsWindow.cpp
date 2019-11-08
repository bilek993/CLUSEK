#include "VehicleDetailsWindow.h"
#include <imgui.h>
#include "../../ECS/Components/VehicleComponent.h"

void VehicleDetailsWindow::Draw()
{
	ImGui::Begin("Vehicle details", &IsEnabled);

	auto vehicleView = Registry->view<VehicleComponent>();

	DrawCombo(vehicleView);
	DrawDetails(vehicleView);

	ImGui::End();
}

void VehicleDetailsWindow::DrawCombo(entt::basic_view<unsigned int, VehicleComponent> &view)
{
	std::string vehicleIds;
	auto counter = 0;

	CurrentVehicleComponent = nullptr;

	view.each([this, &vehicleIds, &counter](VehicleComponent& vehicleComponent)
	{
		vehicleIds += vehicleComponent.VehicleId + '\0';

		if (SelectedId == counter++)
		{
			CurrentVehicleComponent = &vehicleComponent;
		}
	});

	ImGui::Combo("Selected vehicle Id", &SelectedId, vehicleIds.c_str());
}

void VehicleDetailsWindow::DrawDetails(entt::basic_view<unsigned int, VehicleComponent> &view)
{
	ImGui::Separator();

	if (CurrentVehicleComponent == nullptr)
	{
		ImGui::Text("No data!");
	}
	else
	{
		
	}
}
