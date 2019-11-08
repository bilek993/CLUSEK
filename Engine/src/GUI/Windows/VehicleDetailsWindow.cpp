#include "VehicleDetailsWindow.h"
#include <imgui.h>
#include "../../ECS/Components/VehicleComponent.h"

void VehicleDetailsWindow::Draw()
{
	auto vehicleView = Registry->view<VehicleComponent>();
	std::string vehicleIds = "";

	ImGui::Begin("Vehicle details", &IsEnabled);

	vehicleView.each([&vehicleIds](VehicleComponent& vehicleComponent)
	{
		vehicleIds += vehicleComponent.VehicleId + '\0';
	});

	ImGui::Combo("Selected vehicle Id", &SelectedId, vehicleIds.c_str());

	ImGui::End();
}
