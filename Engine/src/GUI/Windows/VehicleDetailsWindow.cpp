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

	view.each([&vehicleIds](VehicleComponent& vehicleComponent)
	{
		vehicleIds += vehicleComponent.VehicleId + '\0';
	});

	ImGui::Combo("Selected vehicle Id", &SelectedId, vehicleIds.c_str());
}

void VehicleDetailsWindow::DrawDetails(entt::basic_view<unsigned int, VehicleComponent> &view)
{
}
